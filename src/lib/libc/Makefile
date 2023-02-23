SRC=init.c syscalls.c $(wildcard stdio/*.c) $(wildcard stdlib/*.c) $(wildcard string/*.c)
CRT=$(wildcard x86_64/*.s)

COMPILE_OBJS = $(patsubst %.c, %.o, $(SRC)) $(patsubst %.s, %.o, $(CRT))
OBJS = $(patsubst %.c, %.o, $(SRC)) # list of object files

all: libc.a

%.o: %.c
	@echo "[  C  ]: $<"
	@$(CC) -c $(CFLAGS) -I include -Wall -o $@ $<

%.o: %.s
	@echo "[ ASM ]: $<"
	@$(CC) -c $(CFLAGS) -I include -Wall -o $@ $<

libc.a: $(COMPILE_OBJS) 
	@echo "[ AR  ]: libc.a"
	@${AR} rcs -o libc.a ${OBJS}

install:
	@mkdir -p ${D}/lib/
	@cp libc.a ${D}/lib/libc.a
	@cp $(patsubst %.s, %.o, $(CRT)) ${D}/lib/

install-headers:
	@mkdir -p ${D}/usr/include
	@cp -R include/* ${D}/usr/include/

clean:
	@rm libc.a
	rm ${COMPILE_OBJS}