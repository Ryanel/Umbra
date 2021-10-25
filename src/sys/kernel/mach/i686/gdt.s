gdt:
.quad 0x0000000000000000
.quad 0x00CF9A000000FFFF
.quad 0x00CF92000000FFFF
.quad 0x00CFFA000000FFFF
.quad 0x00CFF2000000FFFF

gdtr: 
    .word . - gdt - 1 # For limit storage
    .long gdt # For base storage

.global setupGDT

setupGDT:
	lgdt (gdtr)
	jmp $0x08,$.reload_cs
.reload_cs:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ret
