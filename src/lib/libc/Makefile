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
	@mkdir -p ${INSTALL_DIR}/lib/
	@cp libc.a ${INSTALL_DIR}/lib/libc.a
	@cp $(patsubst %.s, %.o, $(CRT)) ${INSTALL_DIR}/lib/

install-headers:
	@mkdir -p ${INSTALL_DIR}/usr/include
	@cp -R include/* ${INSTALL_DIR}/usr/include/

clean:
	@rm libc.a
	rm ${COMPILE_OBJS}