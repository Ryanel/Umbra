.global _start

msg: .ascii "Hello, from a loaded program!\n\0"

_start:
    # Write the buffer with write()
    mov $1, %rax
    mov $1, %rbx
    mov $(msg), %rcx
    mov $31, %rdx
    int $0x80
    # Terminate this application.
    mov $0, %rax
    int $0x80
