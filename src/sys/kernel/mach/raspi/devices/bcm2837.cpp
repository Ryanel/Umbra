#include <kernel/arm/rpi/bcm2837.h>
#include <kernel/arm/rpi/rpi.h>

void kernel::arm::rpi::bcm2837::ic_enable() {
    kernel::arm::rpi::rpi::get().mmio_write(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

unsigned int kernel::arm::rpi::bcm2837::getIRQ() {
    return kernel::arm::rpi::rpi::get().mmio_read(IRQ_PENDING_1);
}
