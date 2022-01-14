all: test_program

test_program:
	@${CC} -o test_program main.c syscall.s

install:
	cp test_program ${DESTDIR}