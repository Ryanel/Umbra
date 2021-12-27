global after_gdt
global setupGDT

setupGDT:
    lgdt    [rdi]

    ; Set segement registers
    push    rax
    mov     ax,     0x10
    mov     ss,     ax
    mov     ds,     ax
    mov     es,     ax
    mov     fs,     ax
    mov     gs,     ax

    ; Swap code segment
    mov     rax,    after_gdt
    push    0x08                    ; New CS
    push    rax                     ; "Return location"
    retfq                           ; Swap!

after_gdt:
    pop rax
    ret

; GDT.
section .rodata
align 16
gdt64:
.null:                           ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).

.kcode:                          ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0b10011010                ; Access (exec/read).
    db 0b10101111                ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).

.kdata:                          ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0b10010010                ; Access (read/write).
    db 0b00000000                ; Granularity.
    db 0                         ; Base (high).

.ucode:                          ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0b11111010                ; Access (exec/read).
    db 0b10101111                ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).

.udata:                          ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0b11110010                ; Access (read/write).
    db 0b00000000                ; Granularity.
    db 0                         ; Base (high).

global gdt64_pointer
gdt64_pointer:
    dw $ - gdt64 - 1
    dq gdt64
