ROOT_SOURCES := $(wildcard *.cpp) $(wildcard debug/*.cpp) $(wildcard hal/*.cpp) $(wildcard log/*.cpp) $(wildcard mm/*.cpp) $(wildcard tasks/*.cpp) $(wildcard vfs/*.cpp) 
ARCH_SOURCES := $(wildcard ${NYX_ARCH}/*.cpp) $(wildcard ${NYX_ARCH}/drivers/*.cpp)
SUBARCH_SOURCES := $(wildcard ${NYX_ARCH}/${NYX_TARGET}/*.cpp) $(wildcard ${NYX_ARCH}/${NYX_TARGET}/*.s)

BUILD_DIR ?= build/kernel

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-declarations \
            -Wredundant-decls -Winline -Wno-long-long \
            -Wconversion

OPT_FLAGS ?=

ALL_SOURCES := ${ROOT_SOURCES} ${ARCH_SOURCES} ${SUBARCH_SOURCES}

ALL_OBJECTS := $(patsubst %.cpp, %.o, $(ALL_SOURCES))
ALL_OBJECTS :=  $(patsubst %.s, %.o, $(ALL_OBJECTS)) 
ALL_OBJECTS := $(ALL_OBJECTS:%=$(BUILD_DIR)/%)
DEPFILES    := $(patsubst %.o,%.d,$(ALL_OBJECTS))

KERNEL_LINKER_FILE := ${NYX_ARCH}/${NYX_TARGET}/linker.ld

LIBRARIES := ${SYSROOT}/sys/lib_kernel.a

CFLAGS ?= -Og -ggdb3 -fno-omit-frame-pointer
CXXFLAGS ?= ${CFLAGS}
LDFLAGS ?= -z max-page-size=0x1000
CXXFLAGS := ${CXXFLAGS} ${OPT_FLAGS} -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-builtin \
			-fno-rtti -fno-exceptions -ffreestanding -fno-builtin -nostdlib -fstack-protector-strong \
 			-DARCH_${NYX_TARGET} -std=c++17 -I ./include -I ./include/stl/ -I ../klibc/include/ -ffreestanding -nostdlib -lgcc
ASMFLAGS ?= -g -f elf64

MKDIR_P := mkdir -p

.PHONY: all install

all: ${BUILD_DIR}/kernel.elf

-include $(DEPFILES)

${BUILD_DIR}/kernel.elf: ${ALL_OBJECTS}
	@echo "[ LD  ]: $@"
	@$(CXX) -T ${KERNEL_LINKER_FILE} -MMD -MP -o $@ ${CXXFLAGS} ${ALL_OBJECTS} ${LIBRARIES} ${LDFLAGS}

symbols:
	@mkdir -p ${INSTALL_DIR}/sys/
	@nm ${BUILD_DIR}/kernel.elf | grep " T " > ${BUILD_DIR}/kernel.sym
	@python3 create-symbols.py ${BUILD_DIR}
	@cp ${BUILD_DIR}/syms.bin ${INSTALL_DIR}/sys/syms.bin

install:
	@cp ${BUILD_DIR}/kernel.elf ${INSTALL_DIR}/sys/

${BUILD_DIR}/%.o: %.s
	@echo "[ ASM ]: $<"
	@$(MKDIR_P) $(dir $@)
	@$(ASM) $(ASMFLAGS) -MD  $($<:%.s=%.d) -MP -o $@ $<

${BUILD_DIR}/%.o: %.cpp
	@echo "[ CXX ]: $<"
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@