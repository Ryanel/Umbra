#include <assert.h>
#include <kernel/boot/boot_file.h>
#include <kernel/boot/stivale2.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/log.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vga_text_console.h>
#include <stddef.h>
#include <stdint.h>
// Stack in BSS
extern uint8_t stack[16384];

kernel::boot::boot_file_container   kernel::boot::g_bootfiles;
kernel::device::serial_text_console con_serial;
kernel::device::fb_text_console     con_fb;
kernel::device::vga_text_console    con_vga;

void init_global_constructors();
// code block.

#pragma region Stivale

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // Same as above.
    .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
            // Instead of 0, we now point to the previous header tag. The order in
            // which header tags are linked does not matter.
            .next = 0},
    // We set all the framebuffer specifics to 0 as we want the bootloader
    // to pick the best it can.
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = 0,
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // Bit 1, if set, causes the bootloader to return to us pointers in the
    // higher half, which we likely want since this is a higher half kernel.
    // Bit 2, if set, tells the bootloader to enable protected memory ranges,
    // that is, to respect the ELF PHDR mandated permissions for the executable's
    // segments.
    // Bit 3, if set, enables fully virtual kernel mappings, which we want as
    // they allow the bootloader to pick whichever *physical* memory address is
    // available to load the kernel, rather than relying on us telling it where
    // to load it.
    // Bit 4 disables a deprecated feature and should always be set.
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    // This header structure is the root of the linked list of header tags and
    // points to the first one in the linked list.
    .tags = (uintptr_t)&framebuffer_hdr_tag};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
void* stivale2_get_tag(struct stivale2_struct* stivale2_struct, uint64_t id) {
    struct stivale2_tag* current_tag = (stivale2_tag*)stivale2_struct->tags;
    for (;;) {
        // If the tag pointer is NULL (end of linked list), we did not find
        // the tag. Return NULL to signal this.
        if (current_tag == NULL) { return NULL; }

        // Check whether the identifier matches. If it does, return a pointer
        // to the matching tag.
        if (current_tag->identifier == id) { return current_tag; }

        // Get a pointer to the next tag in the linked list and repeat.
        current_tag = (stivale2_tag*)current_tag->next;
    }
}

#pragma endregion

void boot_init_log(struct stivale2_struct* svs, kernel::device::fb_text_console& con_fb) {
    auto& log = kernel::log::get();
    log.init(&con_serial);
    log.shouldBuffer = false;  // Disable buffering for now

    // Setup graphics terminal
    auto* fb_info_tag     = (stivale2_struct_tag_framebuffer*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    auto* fb_textmode_tag = (stivale2_struct_tag_textmode*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_TEXTMODE_ID);

    if (fb_info_tag != nullptr) {
        con_fb.framebuffer = sw_framebuffer((uint8_t*)fb_info_tag->framebuffer_addr, fb_info_tag->framebuffer_width,
                                            fb_info_tag->framebuffer_height, fb_info_tag->framebuffer_bpp,
                                            fb_info_tag->framebuffer_pitch, fb_format::bgr);
        log.init(&con_fb);

        kernel::log::debug("display", "Recieved framebuffer: %dx%dx%d @ 0x%p from %s\n", fb_info_tag->framebuffer_width,
                           fb_info_tag->framebuffer_height, fb_info_tag->framebuffer_bpp, fb_info_tag->framebuffer_addr,
                           &svs->bootloader_brand);
    } else {
        assert(fb_textmode_tag != nullptr);
        con_vga.buffer_address = fb_textmode_tag->address;
        log.init(&con_vga);
    }
}

void kernel_print_version() { kernel::log::info("kernel", "Umbra v. %s on x86_64\n", KERNEL_VERSION); }

// The following will be our kernel's entry point.
extern "C" void _start(struct stivale2_struct* stivale2_struct) {
    init_global_constructors();

    boot_init_log(stivale2_struct, con_fb);

    kernel::log::debug("kernel", "Alive!\n");
    kernel_print_version();

    // GDT, IDT

    // PMM, VMM, HEAP

    // Timer
    // pit_timer timer_pit;
    // timer_pit.init();
    // kernel::time::system_timer = &timer_pit;
    // g_idt.enable_interrupts();

    // We're done, just hang...
    for (;;) { asm("hlt"); }
}
