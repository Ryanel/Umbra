.section .multiboot, "ax"
.align 4
# Multiboot Header
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set GRAPHICS, 1<<2
.set FLAGS,    ALIGN | MEMINFO | GRAPHICS
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS) 

.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0
.long 0
.long 0
.long 0
.long 0
.long 1     # Mode
.long 640   # Width
.long 480   # Height
.long 0     # Depth
