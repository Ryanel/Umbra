.intel_syntax noprefix

.global thread_switch
.extern current_tcb
.extern current_task
.extern set_kernel_stack

thread_switch:
    push    ebx
    push    esi
    push    edi
    push    ebp

    # Save previous state
    mov     edi,[current_tcb]       # edi = address of prev tcb
    mov     [edi+(4*4)],esp            # Save stack pointer into tcb

    # Load new state
    mov     esi,[esp+(4+1)*4]       # esi = address of next struct thread
    mov     esp,[esi +(4*4)]           # Load ESP for next task's kernel stack from the thread's TCB

    # Update scheduling information
    mov     [current_tcb],esi       # current_tcb = new TCB
    movb    [esi + (4*7)], 2        # Set state to "running"

    # Now, manipulate the tasks 
    mov     edi, [esi + (4 * 6)]    # Load thread task ptr
    mov     eax, [current_task]     # Load current task ptr
    cmp     eax, edi                # Compare
    je      .switch_vas_done        # No need to swap VAS...

    # Swap to the virtual address space
    mov     eax, [edi + (4 * 4)]    # Load phys addr of VAS from task
    mov     cr3, eax                # Set the active page table to the VAS!
    mov     [current_task], edi     # Set the new active task in the scheduler


.switch_vas_done:
    # Tell the TSS that we switched stacks
    mov     eax, esp
    call    set_kernel_stack
    # Restore runtime state (registers)
    pop     ebp
    pop     edi
    pop     esi
    pop     ebx
    ret                          # Load rest of registers, restart execution.
