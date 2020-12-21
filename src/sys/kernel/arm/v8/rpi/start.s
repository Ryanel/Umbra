// start.s for rpi

// Responsibilities:
// 1. Determines the BP and AP, and puts the APs in an infinite loop.
// 2. Creates a stack for the BP.
// 3. Clears bss
// 4. Launches __kernel_ap_start()
// 5. A infinite loop if the ap fails to start.

.section ".text.boot"

#include <kernel/arm/rpi/regs.h>

.global _start
.global _start_aps
_start:
    // Identify BP and AP. BP is core 0.
    // This may or may not have cores 1-3 running, depending on the firmware version!
    mrs     x1, mpidr_el1
    and     x1, x1, #3                  // x1 = mpidr_el1 & 3
    cbnz    x1, ap_start                // x1 != 0? => ap_start

bp_start:                               // This is core 0. Continue with boot.
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

bp_el3:
    cmp     x0, #12                     // Is the processor in El3?
    bne     bp_el2                      // No, it's in EL2.
    // Go down to EL2.
    mov     x2, #0x5b1
    msr     scr_el3, x2
    mov     x2, #0x3c9
    msr     spsr_el3, x2
    adr     x2, bp_el2
    msr     elr_el3, x2
    eret                                // Jump to bp_el2 in EL2.

bp_el2:
    cmp     x0, #4                      // Are we in EL2?
    beq     bp_enter                    // No, we're in EL1. This can happen in QEMU emulation.
    msr     sp_el1, x1

    // Disable co-processor traps
    mov     x0, #0x33FF
    msr     cptr_el2, x0
    msr     hstr_el2, xzr
    mov     x0, #(3 << 20)
    msr     cpacr_el1, x0

    // Enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr

    // Disable execution level dependent stacks
    msr     SPSel, #0

    // Enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Change to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, bp_enter
    msr     elr_el2, x2
    eret

    // Entry point of the boot processor
bp_enter:
    // Initialise the stack for core 0's boot process
    // Should be in EL1 now.
    ldr     x1, =_start                 // Set x1 (sp) to 0x80000
    mov     sp, x1

    // Clear bss before launch
    ldr     x1, =__kernel_bss_start
    ldr     w2, =__kernel_bss_size
    cbz     w2, 2f                      // Is w2 (size) == 0? Jump out early.
1:  str     xzr, [x1], #8               // Set *x1[0...8] = 0; x1 += 8
    sub     w2, w2, 1                   // w2 -= 1. TODO: Check to see if this is correct.
    cbnz    w2, 1b                      // Loop if w2 != 0

    // Jump to kernel
2:  bl      _kernel_bp_start            // Jump to kernel
    b       _halt                       // Halt on error

    // Entry point of an application processor
ap_start:
    //bl _halt
    mrs     x3, mpidr_el1               // x1 = core count
    and     x3, x1, #3                  // x1 = mpidr_el1 & 3

    // Setup a tiny boot stack for each core
    ldr     x0, =_start                 // Set x1 (sp) to 0x80000
    mov     x2, #2000                   // Space per core
    mul     x1, x2, x3                  // Multiply by the space needed per core stack
    sub     x0, x0, x1                  // Subtract that to get the base pointer of this tack
    mov     sp, x0                      // Set Stack
    
    mov     x0, x3                      // Set core number as the first argument
    bl _kernel_ap_start                 // Start AP

_start_aps:
    ldr x1, =ap_start
    mov x0, #0xE0
    str x1,[x0, 0]
    str x1,[x0 ,8]
    str x1,[x0, 16]
    ret
_halt:
    wfi
    b _halt
