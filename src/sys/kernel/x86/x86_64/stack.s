section .bss
global stack_top
global stack
align 16
stack:
resb 16384 
stack_top:
