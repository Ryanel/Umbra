SRC=$(wildcard stdio/*.c) $(wildcard stdlib/*.c) $(wildcard string/*.c)
OBJS = $(patsubst %.c, %.o, $(SRC)) # list of object files

all: libc_kernel.a

%.o: %.c
	$(CC) -c $(CFLAGS) -I include -ffreestanding -nostdlib -Wall -D__is_libk -o $@ $<

libc_kernel.a: $(OBJS)
	${AR} rcs -o libc_kernel.a ${OBJS}

clean:

install: install_headers
	@mkdir -p ${INSTALL_DIR}/lib/
	@cp libc_kernel.a ${INSTALL_DIR}/lib/libc_kernel.a

install_headers:
	@mkdir -p ${INSTALL_DIR}/usr/include
	@cp -R include/* ${INSTALL_DIR}/usr/include/