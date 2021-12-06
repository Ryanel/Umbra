.global _start

msg: .ascii "Hello, from a loaded program!\n\0"

_start:
    # Write the buffer with write()
    mov $1, %eax
    mov $1, %ebx
    mov $(msg), %ecx
    mov $31, %edx
    int $0x80
    # Terminate this application.
    mov $0, %eax
    int $0x80
