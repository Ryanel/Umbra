.intel_syntax noprefix
.global thread_switch

thread_switch:
    push    ebx
    push    esi
    push    edi
    push    ebp

    # Save previous state
    mov     edi,[current_tcb]    # edi = address of prev tcb
    mov     [edi+ 0],esp         # Save stack pointer into tcb

    # Load new state
    mov     esi,[esp+(4+1)*4]    # esi = address of next struct thread
    mov     esp,[esi+ 0]         #Load ESP for next task's kernel stack from the thread's TCB

    # Update scheduling information
    mov     [current_tcb],esi    # current_tcb = new TCB
    movb    [esi+0], 2           # Set state to "running"

    # Restore runtime state (registers)
    pop     ebp
    pop     edi
    pop     esi
    pop     ebx
    ret                          # Load rest of registers, restart execution.
