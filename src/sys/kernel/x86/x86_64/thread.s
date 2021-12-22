.global thread_switch
thread_switch:
    ret

.global set_page_table
set_page_table:
    mov %rdi, %cr3
    ret
