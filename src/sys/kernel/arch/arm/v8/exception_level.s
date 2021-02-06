.global armv8_cpu_get_exception_level

armv8_cpu_get_exception_level:
    mrs     x0, CurrentEL   // Load CurrentEL into x0
    lsr     x0, x0, #2      // Shift 2 bits right because first 2 bits are reserved
    ret
