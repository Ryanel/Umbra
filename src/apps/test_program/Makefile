all: test_program

test_program:
	${CC} -nostdlib -nostartfiles -o test_program main.s

install:
	cp test_program ${INSTALL_DIR}