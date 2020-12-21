ENTRY(_start)
 
SECTIONS
{
    . = 0x80000;

    __kernel_start = .;

    __kernel_text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*)
    }
    . = ALIGN(4096); /* align to page size */
    __kernel_text_end = .;
 
    __kernel_rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __kernel_rodata_end = .;
 
    __kernel_data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __kernel_data_end = .;
 
    __kernel_bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __kernel_bss_end = .;
    __kernel_bss_size = __kernel_bss_end - __kernel_bss_start;
    __kernel_end = .;
}