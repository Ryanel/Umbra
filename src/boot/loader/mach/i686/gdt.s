.section .data
.global gdt
.global gdtr

gdt:
.quad 0x0000000000000000
.quad 0x00CF9A000000FFFF
.quad 0x00CF92000000FFFF
.quad 0x00CFFA000000FFFF
.quad 0x00CFF2000000FFFF

gdtr: .word . - gdt - 1 # For limit storage
     .long gdt # For base storage
