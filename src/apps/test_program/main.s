.global _start

_start:
    # Do a test system call
    mov $0, %eax
    int $0x80

    # Terminate this application.
    mov $1, %eax
    int $0x80
