#include <kernel/log.h>
#include <kernel/time.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/ports.h>

void pit_timer::init() {
    set_phase(1000);

    klogf("timer", "x86 PIT initialised at %dhz (%d ns resolution)\n", resolution_hz, resolution_ns());
}

void pit_timer::tick() {
    ticks++;
    kernel::time::increment(resolution_ns());
}

uint64_t pit_timer::resolution_ns() { return 1000000000 / resolution_hz; }

void pit_timer::set_phase(unsigned int hz) {
    int divisor = 1193180 / hz;  // Calculate the divisor
    outb(0x43, 0x36);            // Command 0x36
    outb(0x40, divisor & 0xFF);  // Send Divisor Lo
    outb(0x40, divisor >> 8);    // Send Divisor Hi

    resolution_hz = hz;
}
