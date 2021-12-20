ROOT_SOURCES := $(wildcard *.cpp) $(wildcard log/*.cpp) $(wildcard mm/*.cpp) $(wildcard tasks/*.cpp) $(wildcard vfs/*.cpp)
ARCH_SOURCES := $(wildcard ${NYX_ARCH}/*.cpp) $(wildcard ${NYX_ARCH}/drivers/*.cpp)
SUBARCH_SOURCES := $(wildcard ${NYX_ARCH}/${NYX_TARGET}/*.cpp) $(wildcard ${NYX_ARCH}/${NYX_TARGET}/*.s)

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-declarations \
            -Wredundant-decls -Winline -Wno-long-long \
            -Wconversion

ALL_SOURCES := ${ROOT_SOURCES} ${ARCH_SOURCES} ${SUBARCH_SOURCES}

ALL_OBJECTS := $(patsubst %.cpp, %.o, $(ALL_SOURCES))
ALL_OBJECTS :=  $(patsubst %.s, %.o, $(ALL_SOURCES)) 

KERNEL_LINKER_FILE := ${NYX_ARCH}/${NYX_TARGET}/linker.ld

LIBRARIES := ${SYSROOT}/lib/libc_kernel.a

CFLAGS ?= -Og
CXXFLAGS ?= -Og
LDFLAGS ?= -z max-page-size=0x1000
CXXFLAGS := ${CXXFLAGS} -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-builtin \
			-fno-rtti -fno-exceptions -ffreestanding -fno-builtin -nostdlib -fstack-protector-strong \
 			-DARCH_${NYX_TARGET} -std=c++17 -I ./include -I ./include/stl/ -I ${SYSROOT}/usr/include/ -ffreestanding -nostdlib -lgcc

%.o: %.s
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

all: kernel.elf
kernel.elf: ${ALL_OBJECTS}
	$(CXX) -T ${KERNEL_LINKER_FILE} -o $@ ${CXXFLAGS} ${ALL_OBJECTS} ${LIBRARIES} ${LDFLAGS}

install:
	@cp kernel.elf ${INSTALL_DIR}