#include <kernel/arm/rpi/rpi.h>
#include <kernel/delay.h>
#include <kernel/mem_utils.h>

using namespace kernel::arm::rpi;

/// Global Raspberry Pi Object
rpi raspi;

rpi& rpi::get() { return raspi; }
int  rpi::get_version() { return raspi_version; }

uintptr_t    rpi::mmio_base() { return mmio_addr; }
unsigned int rpi::mmio_read(unsigned int offset) { return *(volatile uint32_t*)(mmio_addr + offset); }
void rpi::mmio_write(unsigned int offset, unsigned int value) { *(volatile uint32_t*)(mmio_addr + offset) = value; }

void rpi::init() {
    // Detect which raspberry pi we're running on (it's 3 or 4)
    uint32_t reg;
    asm volatile("mrs %0, midr_el1" : "=r"(reg));

    switch ((reg >> 4) & 0xFFF) {
        case 0xD03:  // Raspberry PI 3
            raspi_version = 3;
            mmio_addr     = 0x3F000000;
            break;
        case 0xD08:  // Raspberry Pi 4
            raspi_version = 4;
            mmio_addr     = 0xFE000000;
            break;
        default:
            // TODO: Crash!
            raspi_version = 4;
            mmio_addr     = 0xFE000000;
    }
}

int rpi::mbox_call(unsigned char channel) {
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (channel & 0xF));

    kernel::memory_utils::spin_flag_unset(mmio_addr + MBOX_STATUS, MBOX_FULL);  // Wait until full flag clear.
    mmio_write(MBOX_WRITE, r);                                                  // Do call to mailbox

    // Wait for a response...
    while (true) {
        kernel::memory_utils::spin_flag_unset(mmio_addr + MBOX_STATUS, MBOX_EMPTY);  // Wait until empty flag clear.
        // Find out if we were successful
        if (r == mmio_read(MBOX_READ)) { break; }
    }

    /// \returns 1 if successful, 0 if failed.
    return mbox[1] == MBOX_RESPONSE;
}