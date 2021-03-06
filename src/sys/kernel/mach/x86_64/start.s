# Start
.section .text
.global _start
.extern _halt
.extern stack_top
.extern kernel_entry

.type _start, @function
_start:
	# Set stack top 
	movabs $stack_top, %rsp
	movabs $stack_top, %rbp

	# Push GRUB multiboot paramaters
	#pushl %ebx
	#pushl %eax

	# Call the kernel
	call kernel_entry
	
	# If we somehow reach here, call _halt() to halt the current processor.
	jmp _halt
.size _start, . - _start
