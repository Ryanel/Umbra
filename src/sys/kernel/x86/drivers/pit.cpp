#include <kernel/log.h>
#include <kernel/time.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/ports.h>

void pit_timer::init() {
    kernel::interrupts::handler_register(32, this);  // Register for IRQ0
    set_phase(1000);
    kernel::log::debug("timer", "x86 PIT initialised at %d ns resolution\n", resolution_ns());
}

void pit_timer::tick() {
    ticks++;
    kernel::time::increment(resolution_ns());
}

bool pit_timer::handle_interrupt(register_frame_t* regs) {
    tick();
    return true;
}

uint64_t pit_timer::resolution_ns() { return 1000000000 / resolution_hz; }

void pit_timer::set_phase(unsigned int hz) {
    int divisor = 1193180 / hz;             // Calculate the divisor
    outb(0x43, 0x36);                       // Command 0x36
    outb(0x40, (uint8_t)(divisor & 0xFF));  // Send Divisor Lo
    outb(0x40, (uint8_t)(divisor >> 8));    // Send Divisor Hi

    resolution_hz = hz;
}
