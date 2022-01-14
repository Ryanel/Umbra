%include "x86/x86_64/regs.asminc"

extern current_tcb
extern current_task
extern set_kernel_stack

TCB_STACK_OFF  EQU 24
TCB_TASK_OFF   EQU 40
TCB_STATUS_OFF EQU 48
TASK_VAS_OFF   EQU 24

global thread_switch
extern swap_vas

; void thread_switch(thread* next)
; rdi = next
thread_switch:
    ; rdi = Address of New TCB 
    rframe_save

    ; Current thread = New thread
    mov       rbx, [current_tcb]                      ; rbx = Address of Previous TCB
    mov       [rbx + TCB_STACK_OFF], rsp              ; Save previous stack

    mov       [current_tcb], rdi                      ; Current_tcb = new TCB
    mov       rsp, [rdi + TCB_STACK_OFF]              ; Load RSP for next task's kernel stack from the new TCB
    mov qword [rdi + TCB_STATUS_OFF], 2               ; Set state to "running"

    ; Swap VAS
    mov     r12, [rdi + TCB_TASK_OFF]                 ; Load thread task ptr
    mov     r13, [current_task]                       ; Load current task ptr
    cmp     r12, r13                                  ; Compare
    je      .switch_vas_done                          ; No need to swap VAS...

    mov       rbx, [r12 + TASK_VAS_OFF]               ; Load phys addr of VAS from task
    mov       cr3, rbx                                ; Set the active page table to the VAS!
    mov       [current_task], r12                     ; Set the new active task in the scheduler

.switch_vas_done:
    ; Restore registers (Sys-V ABI)
    rframe_load
    ret

global set_page_table
set_page_table:
    mov cr3, rdi
    ret


global enter_usermode
enter_usermode:
    push 0x23
    push rsi
    pushfq
    push 0x1B
    push rdi
    iretq