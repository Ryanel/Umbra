# Kernel ----------------------------------------------------------------------

# Source files
file(GLOB KERNEL_SOURCES_ROOT CONFIGURE_DEPENDS *.c *.cpp *.s include/kernel/*.h)
file(GLOB KERNEL_SOURCES_ARCH CONFIGURE_DEPENDS arch/${BUILD_ARCH}/*.c arch/${BUILD_ARCH}/*.cpp arch/${BUILD_ARCH}/*.s includes/kernel/${BUILD_ARCH}/*.h)
file(GLOB_RECURSE KERNEL_SOURCES_ARCHVER CONFIGURE_DEPENDS arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.c arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.cpp arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.s includes/kernel/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.h)
file(GLOB_RECURSE KERNEL_SOURCES_MACH CONFIGURE_DEPENDS mach/${BUILD_BOARD}/*.c mach/${BUILD_BOARD}/*.cpp mach/${BUILD_BOARD}/*.s includes/kernel/${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.h)

set (KERNEL_CRTI mach/${BUILD_BOARD}/crt/crti.s)
set (KERNEL_CRTN mach/${BUILD_BOARD}/crt/crtn.s)

set(KERNEL_SOURCES ${KERNEL_CRTI} ${CRTBEGIN_O}  ${KERNEL_SOURCES_ROOT} ${KERNEL_SOURCES_ARCH} ${KERNEL_SOURCES_ARCHVER} ${KERNEL_SOURCES_MACH} ${CRTEND_O} ${KERNEL_CRTN})

execute_process( COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtbegin.o OUTPUT_VARIABLE CRTBEGIN_O OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process( COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtend.o OUTPUT_VARIABLE CRTEND_O OUTPUT_STRIP_TRAILING_WHITESPACE)

# Configuration
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_BINARY_DIR}/include/kernel/config.h"
)

# Headers and include files
file(GLOB_RECURSE PUBLIC_HEADER_FILES CONFIGURE_DEPENDS "{CMAKE_CURRENT_SOURCE_DIR}/include/public/*.h")
include_directories(./include)
include_directories("${PROJECT_BINARY_DIR}/include")
# Linking ---------------------------------------------------------------------

# Link the kernel with these options
set (KERNEL_LINK_FLAGS "-ffreestanding -fno-builtin -nostdlib -static ${CMAKE_C_FLAGS} ${KERNEL_ADDITIONAL_LINK_FLAGS}")
if (${BUILD_HAS_BOARD})
    add_link_options(SHELL:${KERNEL_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/mach/${BUILD_BOARD}/linker.ld)
else()
    add_link_options(${KERNEL_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/linker.ld)
endif()

add_executable(kernel ${KERNEL_SOURCES})
set_target_properties(kernel    PROPERTIES 
                                PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
                                CXX_STANDARD 17
                                CXX_EXTENSIONS OFF)
target_link_libraries(kernel PUBLIC c_kernel gcc)

# Compilation -----------------------------------------------------------------

# Do not allow builtin functions, libraries, or standard include. We're also freestanding.
target_compile_options(kernel PRIVATE -Wall -fno-rtti -fno-exceptions -ffreestanding -fno-builtin -nostdlib) 

# Installation ----------------------------------------------------------------
set_target_properties(
    kernel
    PROPERTIES 
        OUTPUT_NAME "kernel"
        SUFFIX ".elf"
)

install(TARGETS kernel 
    DESTINATION sys/
    PUBLIC_HEADER DESTINATION usr/include/kernel/
)