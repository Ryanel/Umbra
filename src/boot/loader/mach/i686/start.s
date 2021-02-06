# Start
.section .text

# External
.extern _halt
.extern stack_top
.extern loader_main
.extern gdtr

.global _start
.type _start, @function
_start:
	# Set stack top 
	movl $stack_top, %esp
	movl $stack_top, %esp

	# Push GRUB multiboot paramaters
	pushl %ebx
	pushl %eax

	# Initialize the GDT
	lgdt (gdtr)
	jmp $0x08,$.reload_cs
.reload_cs:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	# Call loader_main()
	call loader_main
	
	# If we somehow reach here, call _halt() to halt the current processor.
	jmp _halt
.size _start, . - _start
