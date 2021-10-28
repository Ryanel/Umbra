# Start
.section .text

.extern _halt
.extern stack_top
.extern kernel_entry
.extern setupGDT
.extern _init

.global _start
.type _start, @function
_start:
	# Set stack top 
	movl $stack_top, %esp
	movl $stack_top, %esp

	# Push GRUB multiboot paramaters
	pushl %ebx
	pushl %eax

	call setupGDT

	# Global constructors
	call _init

	# Call the kernel
	call kernel_entry
	
	# If we somehow reach here, call _halt() to halt the current processor.
	jmp _halt
.size _start, . - _start
