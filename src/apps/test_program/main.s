.global _start

_start:
    # Terminate this application.
    mov $0, %eax
    int $0x80
