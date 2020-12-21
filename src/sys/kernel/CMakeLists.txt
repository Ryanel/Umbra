# Kernel ----------------------------------------------------------------------

# Source files
file(GLOB KERNEL_SOURCES_ROOT CONFIGURE_DEPENDS *.c *.cpp *.s include/kernel/*.h)
file(GLOB KERNEL_SOURCES_ARCH CONFIGURE_DEPENDS ${BUILD_ARCH}/*.c ${BUILD_ARCH}/*.cpp ${BUILD_ARCH}/*.s includes/kernel/${BUILD_ARCH}/*.h)
file(GLOB KERNEL_SOURCES_ARCHVER CONFIGURE_DEPENDS ${BUILD_ARCH}/${BUILD_ARCH_VER}/*.c ${BUILD_ARCH}/${BUILD_ARCH_VER}/*.cpp ${BUILD_ARCH}/${BUILD_ARCH_VER}/*.s includes/kernel/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.h)
file(GLOB_RECURSE KERNEL_SOURCES_BOARD CONFIGURE_DEPENDS ${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.c ${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.cpp ${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.s includes/kernel/${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.h)

set(KERNEL_SOURCES ${KERNEL_SOURCES_ROOT} ${KERNEL_SOURCES_ARCH} ${KERNEL_SOURCES_ARCHVER} ${KERNEL_SOURCES_BOARD})

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
set (KERNEL_LINK_FLAGS -ffreestanding -fno-builtin -nostdlib -static)
if (${BUILD_HAS_BOARD})
    add_link_options(${KERNEL_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/linker.ld)
else()
    add_link_options(${KERNEL_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_ARCH}/${BUILD_ARCH_VER}/linker.ld)
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