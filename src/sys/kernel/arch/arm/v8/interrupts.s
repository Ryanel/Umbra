#define STACK_FRAME_SIZE 256
#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define IRQ_INVALID_EL1h		5 
#define FIQ_INVALID_EL1h		6 
#define ERROR_INVALID_EL1h		7 

#define SYNC_INVALID_EL0_64	    	8 
#define IRQ_INVALID_EL0_64	    	9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64		11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32		15 

.extern _halt
.global kernel_interrupt_exception

// Save a kernel thread state to the stack at SP
.macro  kstate_save
    sub	    sp, sp, #STACK_FRAME_SIZE
    stp	    x0, x1, [sp, #16 * 0]
    stp	    x2, x3, [sp, #16 * 1]
    stp	    x4, x5, [sp, #16 * 2]
    stp	    x6, x7, [sp, #16 * 3]
    stp	    x8, x9, [sp, #16 * 4]
    stp	    x10, x11, [sp, #16 * 5]
    stp	    x12, x13, [sp, #16 * 6]
    stp	    x14, x15, [sp, #16 * 7]
    stp	    x16, x17, [sp, #16 * 8]
    stp	    x18, x19, [sp, #16 * 9]
    stp	    x20, x21, [sp, #16 * 10]
    stp	    x22, x23, [sp, #16 * 11]
    stp	    x24, x25, [sp, #16 * 12]
    stp	    x26, x27, [sp, #16 * 13]
    stp	    x28, x29, [sp, #16 * 14]
    str	    x30, [sp, #16 * 15] 
.endm

// Load a kernel thread state frame from the stack at sp.
.macro  kstate_load
    stp	    x0, x1, [sp, #16 * 0]
    stp	    x2, x3, [sp, #16 * 1]
    stp	    x4, x5, [sp, #16 * 2]
    stp	    x6, x7, [sp, #16 * 3]
    stp	    x8, x9, [sp, #16 * 4]
    stp	    x10, x11, [sp, #16 * 5]
    stp	    x12, x13, [sp, #16 * 6]
    stp	    x14, x15, [sp, #16 * 7]
    stp	    x16, x17, [sp, #16 * 8]
    stp	    x18, x19, [sp, #16 * 9]
    stp	    x20, x21, [sp, #16 * 10]
    stp	    x22, x23, [sp, #16 * 11]
    stp	    x24, x25, [sp, #16 * 12]
    stp	    x26, x27, [sp, #16 * 13]
    stp	    x28, x29, [sp, #16 * 14]
    str	    x30, [sp, #16 * 15] 
    add	    sp, sp, #STACK_FRAME_SIZE
.endm

// Defines a entry in the vector table
.macro ventry label
    .align	7
    b	\label
.endm

.macro handle_invalid_entry type
    kstate_save
    mov	    x0, #\type
    mrs	    x1, esr_el1
    mrs	    x2, elr_el1
    bl	    kernel_interrupt_exception
    b	    _halt
.endm

.align	11
.globl vectors 
vectors:
	ventry	sync_invalid_el1t			// Synchronous EL1t
	ventry	irq_invalid_el1t			// IRQ EL1t
	ventry	fiq_invalid_el1t			// FIQ EL1t
	ventry	error_invalid_el1t			// Error EL1t

	ventry	sync_invalid_el1h			// Synchronous EL1h
	ventry	el1_irq					    // IRQ EL1h
	ventry	fiq_invalid_el1h			// FIQ EL1h
	ventry	error_invalid_el1h			// Error EL1h

	ventry	sync_invalid_el0_64			// Synchronous 64-bit EL0
	ventry	irq_invalid_el0_64			// IRQ 64-bit EL0
	ventry	fiq_invalid_el0_64			// FIQ 64-bit EL0
	ventry	error_invalid_el0_64			// Error 64-bit EL0

	ventry	sync_invalid_el0_32			// Synchronous 32-bit EL0
	ventry	irq_invalid_el0_32			// IRQ 32-bit EL0
	ventry	fiq_invalid_el0_32			// FIQ 32-bit EL0
	ventry	error_invalid_el0_32			// Error 32-bit EL0
// End of table...

// Interrupt handlers
sync_invalid_el1t:
	handle_invalid_entry  SYNC_INVALID_EL1t

irq_invalid_el1t:
	handle_invalid_entry  IRQ_INVALID_EL1t

fiq_invalid_el1t:
	handle_invalid_entry  FIQ_INVALID_EL1t

error_invalid_el1t:
	handle_invalid_entry  ERROR_INVALID_EL1t

sync_invalid_el1h:
	handle_invalid_entry  SYNC_INVALID_EL1h

fiq_invalid_el1h:
	handle_invalid_entry  FIQ_INVALID_EL1h

error_invalid_el1h:
	handle_invalid_entry  ERROR_INVALID_EL1h

sync_invalid_el0_64:
	handle_invalid_entry  SYNC_INVALID_EL0_64

irq_invalid_el0_64:
	handle_invalid_entry  IRQ_INVALID_EL0_64

fiq_invalid_el0_64:
	handle_invalid_entry  FIQ_INVALID_EL0_64

error_invalid_el0_64:
	handle_invalid_entry  ERROR_INVALID_EL0_64

sync_invalid_el0_32:
	handle_invalid_entry  SYNC_INVALID_EL0_32

irq_invalid_el0_32:
	handle_invalid_entry  IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
	handle_invalid_entry  FIQ_INVALID_EL0_32

error_invalid_el0_32:
	handle_invalid_entry  ERROR_INVALID_EL0_32

el1_irq:
	kstate_save 
	bl	kernel_interrupt_irq
	kstate_load
    eret

.globl kernel_interrupt_init
kernel_interrupt_init:
    adr    x0, vectors        // load VBAR_EL1 with virtual
    msr    vbar_el1, x0        // vector table address
    ret

.globl kernel_interrupt_enable
kernel_interrupt_enable:
    msr    daifclr, #2
    ret

.globl kernel_interrupt_disable
kernel_interrupt_disable:
    msr    daifset, #2
    ret