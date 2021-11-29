# Start

.extern _halt
.extern stack_top
.extern k_bss_start
.extern k_bss_end

.extern _init
.extern setupGDT
.extern kernel_entry

.section .bss, "aw", @nobits
.align 4096
.global boot_page_directory
.global boot_page_table1
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096

.global stack_top
.align 16
stack_bottom:
	.skip 16384 
stack_top:


.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	cld												# Ensure direction flag is clear, SysV compat.
	movl 	$(boot_page_table1 - 0xC0000000), %edi 	# We're going to set the kernel up in a higher half address (0xC0000000)
	movl 	$0, %esi								# Map up to the first 1024 pages, up to the end of the kernel
	movl 	$1023, %ecx
1:
	cmpl 	$(_kernel_end - 0xC0000000), %esi		# Stop at the end of the kernel
	jge 	2f

	movl 	%esi, %edx
	orl 	$0x003, %edx							# Access Bits, Present Writable
	movl 	%edx, (%edi)							# Map this page

	addl 	$4096, %esi								# Size of page is 4096 bytes.
	addl 	$4, %edi								# Next pagetable entry

	loop 1b											# Keep looping
2:
	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# Map the page directory to itself @ 0xFFFFF000
	# movl $(boot_page_directory - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + (4096 - 4)

	# Set the page directory
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl 	%cr0, %ecx
	orl 	$0x80010000, %ecx
	movl 	%ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea 	kernel_early_boot, %ecx
	jmp 	*%ecx
# End of _start

.section .text
kernel_early_boot:
	# Un-identity map the boot page directory
	movl $0, boot_page_directory + 0
	movl %cr3, %ecx
	movl %ecx, %cr3

	# Set the stack
	movl 	$stack_top, %esp
	xor 	%ebp, %ebp       						# EBP needs to be null for end-of-stack

	addl 	$0xC0000000, %ebx						# Add virtual to multiboot magic
	pushl 	%ebx									# GRUB Multiboot info
	pushl 	%eax									# GRUB Multiboot magic

	# System Setup, ready for C
	#call 	setupGDT								# Init the GDT
	call 	_init									# Global constructors
	call 	kernel_entry							# Call the kernel
	jmp 	_halt									# If we somehow reach here, call _halt() to halt the current processor.

.global _halt
_halt:
	cli
1:	hlt
	jmp 1b

.global setupGDT
setupGDT:
	lgdt (%eax)
	jmp $0x08,$.reload_cs
.reload_cs:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	mov $0x2B, %ax
	ltr %ax
	ret
