ENTRY(_start)

SECTIONS
{
	. = 0x00100000;

	_kernel_start = .;

	.multiboot BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
	}

	.multiboot.data : {
		*(.multiboot.data)
	}

	.multiboot.text : {
		*(.multiboot.text)
	}


	. += 0xC0000000;

	.text BLOCK(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}


	.rodata BLOCK(4K) :  AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)

        start_ctors = .;
        *(SORT(.ctors*))  /* Note the "SORT" */
        end_ctors = .;

        start_dtors = .;
        *(SORT(.dtors*))
        end_dtors = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
    
	}

	.data BLOCK(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss BLOCK(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	_kernel_end = .;

	.DISCARD :
	{
		*(.COMMENT)
		*(.comment)
		*(.note.gnu.build-id)
	}
}
