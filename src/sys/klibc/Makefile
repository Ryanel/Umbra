SRC=$(wildcard stdio/*.c) $(wildcard stdlib/*.c) $(wildcard string/*.c)
OBJS = $(patsubst %.c, %.o, $(SRC)) # list of object files

CFLAGS := -O0 -ggdb3

all: lib_kernel.a

%.o: %.c
	$(CC) -c $(CFLAGS) -I include -ffreestanding -nostdlib -Wall -D__is_libk -o $@ $<

lib_kernel.a: $(OBJS)
	${AR} rcs -o lib_kernel.a ${OBJS}

clean:

install:
	@mkdir -p ${D}/sys/
	@cp lib_kernel.a ${D}/sys/lib_kernel.a

install-headers:
	@mkdir -p ${D}/sys/include
	@cp -R include/* ${D}/sys/include/