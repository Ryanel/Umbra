.intel_syntax noprefix
.extern after_gdt
.global setupGDT

setupGDT:
    lgdt    [rdi]

    # Set segement registers
    push    rax
    mov     ax,     0x10
    mov     ss,     ax
    mov     ds,     ax
    mov     es,     ax
    mov     fs,     ax
    mov     gs,     ax

    # Swap code segment
    movabs  rax, offset after_gdt
    pushq   0x08            # New CS
    push    rax             # "Return location"
    retfq                   # Swap!

.global after_gdt
after_gdt:
    pop rax
    ret

# GDT.
.section .rodata
.align 16
gdt64:
    .null:                          # The null descriptor.
    .word 0xFFFF                    # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0                         # Access.
    .byte 1                         # Granularity.
    .byte 0                         # Base (high).

    .kcode:                         # The code descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0b10011010                # Access (exec/read).
    .byte 0b10101111                # Granularity, 64 bits flag, limit19:16.
    .byte 0                         # Base (high).

    .kdata:                         # The data descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0b10010010                # Access (read/write).
    .byte 0b00000000                # Granularity.
    .byte 0                         # Base (high).

    .ucode:                         # The code descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0b11111010                # Access (exec/read).
    .byte 0b10101111                # Granularity, 64 bits flag, limit19:16.
    .byte 0                         # Base (high).

    .udata:                         # The data descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0b11110010                # Access (read/write).
    .byte 0b00000000                # Granularity.
    .byte 0                         # Base (high).

.global gdt64_pointer
gdt64_pointer:
    .word gdt64_pointer - gdt64 - 1
    .quad gdt64
