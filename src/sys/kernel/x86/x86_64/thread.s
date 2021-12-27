extern current_tcb
extern current_task
extern set_kernel_stack

PTR_SIZE EQU 8
OBJ_OFFSET EQU (PTR_SIZE * 4)

TCB_STACK_OFF  EQU (OBJ_OFFSET + ( PTR_SIZE * 0))
TCB_TASK_OFF   EQU (OBJ_OFFSET + ( PTR_SIZE * 2))
TCB_STATUS_OFF EQU (OBJ_OFFSET + ( PTR_SIZE * 3))
TASK_VAS_OFF   EQU (OBJ_OFFSET + ( PTR_SIZE * 0))

global thread_switch
extern swap_vas

thread_switch:
    ; rdi = Address of New TCB 
    ; Save registers (Sys-5 ABI)
    push rbx
    push rsp
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; Current thread = New thread
    mov      rbx, [current_tcb]                     ; rbx = Address of Previous TCB
    mov      [rbx + TCB_STACK_OFF], rsp             ; Save previous stack

    mov      [current_tcb], rdi                     ; Current_tcb = new TCB
    mov      rsp, [rdi + TCB_STACK_OFF]             ; Load RSP for next task's kernel stack from the new TCB
    mov qword [rdi + TCB_STATUS_OFF], 2             ; Set state to "running"

    ; rdi = Address of new TCB

    ; Swap VAS
    ;mov      rbx, [r12 + TASK_VAS_OFF]              ; Load phys addr of VAS from task
    ;mov      cr3, rbx                               ; Set the active page table to the VAS!
    ;mov      [current_task], r12                    ; Set the new active task in the scheduler

    ; Restore registers (Sys-V ABI)
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rsp
    pop rbx
    ret

global set_page_table
set_page_table:
    mov cr3, rdi
    ret
