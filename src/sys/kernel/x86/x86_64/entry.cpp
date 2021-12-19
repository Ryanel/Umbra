#include <assert.h>
#include <kernel/boot/boot_file.h>
#include <kernel/boot/stivale2.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/log.h>
#include <kernel/time.h>
#include <stddef.h>
#include <stdint.h>

// x86
#include <kernel/x86/descriptor_table.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vga_text_console.h>

// Stack in BSS
extern uint8_t stack[16384];

kernel::boot::boot_file_container   kernel::boot::g_bootfiles;
kernel::device::serial_text_console con_serial;
kernel::device::fb_text_console     con_fb;
kernel::device::vga_text_console    con_vga;
x86_idt                             g_idt;

void init_global_constructors();
// code block.

static struct stivale2_header_tag_smp smp_hdr_tag = {
    // Same as above.
    .tag   = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = 0},
    .flags = 0,
};

static struct stivale2_header_tag_any_video tag_any_video = {
    // Same as above.
    .tag        = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = (uintptr_t)&smp_hdr_tag},
    .preference = 0,
};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // Same as above.
    .tag                = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = (uintptr_t)&tag_any_video},
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 24};

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack       = (uintptr_t)stack + sizeof(stack),
    .flags       = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags        = (uintptr_t)&framebuffer_hdr_tag};

void* stivale2_get_tag(struct stivale2_struct* stivale2_struct, uint64_t id) {
    struct stivale2_tag* current_tag = (stivale2_tag*)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) { return NULL; }
        if (current_tag->identifier == id) { return current_tag; }
        current_tag = (stivale2_tag*)current_tag->next;
    }
}

void boot_init_log(struct stivale2_struct* svs, kernel::device::fb_text_console& con_fb) {
    auto& log = kernel::log::get();
    log.init(&con_serial);
    log.shouldBuffer = false;  // Disable buffering for now

    kernel::log::debug("kernel", "Kernel is alive!\n");

    // Setup graphics terminal
    auto* fb_info_tag     = (stivale2_struct_tag_framebuffer*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    auto* fb_textmode_tag = (stivale2_struct_tag_textmode*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_TEXTMODE_ID);

    if (fb_info_tag != nullptr) {
        con_fb.framebuffer = sw_framebuffer((uint8_t*)fb_info_tag->framebuffer_addr, fb_info_tag->framebuffer_width,
                                            fb_info_tag->framebuffer_height, fb_info_tag->framebuffer_bpp,
                                            fb_info_tag->framebuffer_pitch, fb_format::bgr);
        log.init(&con_fb);

        kernel::log::debug("display", "Framebuffer: %dx%dx%d @ 0x%p from %s\n", fb_info_tag->framebuffer_width,
                           fb_info_tag->framebuffer_height, fb_info_tag->framebuffer_bpp, fb_info_tag->framebuffer_addr,
                           &svs->bootloader_brand);
    } else {
        assert(fb_textmode_tag != nullptr);
        con_vga.buffer_address = fb_textmode_tag->address;
        log.init(&con_vga);
    }
    kernel::log::debug("kernel", "Log initialsing");
    kernel::log::status_log("Done", 0xA);
}

void kernel_print_version() { kernel::log::info("kernel", "Umbra v. %s on x86_64\n", KERNEL_VERSION); }

extern "C" void ap_start() {
    kernel::log::trace("cpu", "AP initialised.\n");
    while (true) {}
}

// The following will be our kernel's entry point.
extern "C" void _start(struct stivale2_struct* stivale2_struct) {
    init_global_constructors();

    boot_init_log(stivale2_struct, con_fb);

    kernel_print_version();

    // Start up APs...
    /*
    auto* smp_tag = (stivale2_struct_tag_smp*)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_SMP_ID);
    if (smp_tag != nullptr) {
        kernel::log::info("smp", "%d cores detected\n", smp_tag->cpu_count);
        for (size_t i = 0; i < smp_tag->cpu_count; i++) {
            smp_tag->smp_info[i].target_stack = (uint64_t)&boot_stack + 1024 + (1024 * i);
            smp_tag->smp_info[i].goto_address = (uint64_t)&ap_start;
        }
    }
    */
    // GDT, IDT
    kernel::x86::g_gdt.init();  // Initialise the GDT
    g_idt.init();               // Initialise the IDT
    // PMM, VMM, HEAP

    // Timer
    pit_timer timer_pit;
    timer_pit.init();
    kernel::time::system_timer = &timer_pit;
    g_idt.enable_interrupts();

    // We're done, just hang...
    kernel::log::warn("kernel", "Finished\n");
    for (;;) { asm("hlt"); }
}
