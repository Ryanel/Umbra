#include <kernel/boot/multiboot.h>
#include <kernel/config.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/types.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/paging.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>

extern "C" void      _halt();
extern "C" uint32_t* boot_page_directory;
extern "C" uint32_t* boot_page_table1;
extern "C" uint32_t* _kernel_end;

x86_idt                             g_idt;
kernel::device::vga_text_console    con_vga;
kernel::device::serial_text_console con_serial;
page_directory                      boot_directory;

void kernel_main();
void kernel_print_version() { klogf("kernel", "Umbra v. %s on x86 (i686)\n", KERNEL_VERSION); }

/// The responsibility of the kernel_entry function is to initialse the system into the minimuim startup state.
/// All architecture specific core functions (Tables, Paging, APs, Display) should be setup before control is transfered
/// to kernel_main
extern "C" void kernel_entry(uint32_t mb_magic, multiboot_info_t* mb_info) {
    kernel::device::fb_text_console con_fb;

    boot_directory                = page_directory((page_directory_raw_t*)(&boot_page_directory));
    boot_directory.directory_addr = (uint32_t)(&boot_page_directory) - 0xC0000000;
    boot_directory.pt_virt[768]   = (uint32_t)(&boot_page_table1);
    // Initialise logging
    auto& log = kernel::log::get();
    con_serial.init();
    log.init(&con_serial);
    log.shouldBuffer = false;  // Disable buffering for now

    // Initialise hardware
    // Parse multiboot
    if (mb_magic != 0x2BADB002) {
        klogf("multiboot", "Multiboot magic was 0x%08x, halting!\n", mb_magic);
        panic("Multiboot magic incorrect");
    }

    // Initialise the memory map (get it from GRUB)
    multiboot_memory_map_t* mb_mmap = (multiboot_memory_map_t*)(mb_info->mmap_addr + 0xC0000000);
    for (; (unsigned long)mb_mmap < (mb_info->mmap_addr + 0xC0000000) + mb_info->mmap_length;
         mb_mmap = (multiboot_memory_map_t*)((unsigned long)mb_mmap + mb_mmap->size + sizeof(mb_mmap->size))) {
        uint32_t                addr     = mb_mmap->addr;
        uint32_t                end_addr = mb_mmap->addr + mb_mmap->len - 1;
        kernel::pmm_region_type type     = kernel::pmm_region_type::unknown;
        if (mb_mmap->type == MULTIBOOT_MEMORY_AVAILABLE) { type = kernel::pmm_region_type::ram; }
        kernel::g_pmm.add_region(kernel::pmm_region(type, addr, end_addr));
    }
    kernel::g_vmm.dir_current = &boot_directory;
    kernel::g_pmm.init();
    g_heap.init(false, (uint32_t)(&_kernel_end));

    // Initialise the IDT
    g_idt.init();
    g_idt.enable_interrupts();

    // Initialise the display
    if (mb_info->framebuffer_type == 2) {
        klogf("display", "Using VGA 80x25 textmode\n");
        con_vga.init();
        log.init(&con_vga);
    } else {
        klogf("display", "Recieved framebuffer: %dx%dx%d @ 0x%p from multiboot\n", mb_info->framebuffer_width,
              mb_info->framebuffer_height, mb_info->framebuffer_bpp, mb_info->framebuffer_addr);

        for (size_t i = 0; i < mb_info->framebuffer_height * mb_info->framebuffer_pitch; i += 0x1000) {
            kernel::g_vmm.mmap_direct((virt_addr_t)mb_info->framebuffer_addr + i, (phys_addr_t)mb_info->framebuffer_addr + i,
                                      0x03);
        }

        con_fb.framebuffer = sw_framebuffer((uint8_t*)mb_info->framebuffer_addr, mb_info->framebuffer_width,
                                            mb_info->framebuffer_height, mb_info->framebuffer_bpp, mb_info->framebuffer_pitch);
        log.init(&con_fb);
        con_fb.init();
    }

    kernel_print_version();

    // Initialise a timer
    pit_timer timer_pit;
    timer_pit.init();
    kernel::time::system_timer = &timer_pit;

    // Call into the kernel now that all supported hardware is initialised.
    kernel_main();
}
