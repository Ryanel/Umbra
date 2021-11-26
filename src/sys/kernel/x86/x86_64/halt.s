.global _halt
_halt:
	cli
1:	hlt
	jmp 1b
