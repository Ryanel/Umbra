# Loader ----------------------------------------------------------------------

# Source files
file(GLOB KERNEL_SOURCES_ROOT CONFIGURE_DEPENDS *.c *.cpp *.s include/kernel/*.h)
file(GLOB KERNEL_SOURCES_ARCH CONFIGURE_DEPENDS arch/${BUILD_ARCH}/*.c arch/${BUILD_ARCH}/*.cpp arch/${BUILD_ARCH}/*.s includes/kernel/${BUILD_ARCH}/*.h)
file(GLOB_RECURSE KERNEL_SOURCES_ARCHVER CONFIGURE_DEPENDS arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.c arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.cpp arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.s includes/kernel/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.h)
file(GLOB_RECURSE KERNEL_SOURCES_MACH CONFIGURE_DEPENDS mach/${BUILD_BOARD}/*.c mach/${BUILD_BOARD}/*.cpp mach/${BUILD_BOARD}/*.s includes/kernel/${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.h)
set(LOADER_SOURCES ${KERNEL_SOURCES_ROOT} ${KERNEL_SOURCES_ARCH} ${KERNEL_SOURCES_ARCHVER} ${KERNEL_SOURCES_MACH})

# Configuration
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_BINARY_DIR}/include/loader/config.h"
)

# Headers and include files
include_directories(./include)
include_directories("${PROJECT_BINARY_DIR}/include")
# Linking ---------------------------------------------------------------------

# Link the loader with these options
set (KERNEL_LINK_FLAGS -ffreestanding -fno-builtin -nostdlib -static)
if (${BUILD_HAS_BOARD})
    add_link_options(${KERNEL_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/mach/${BUILD_BOARD}/linker.ld)
else()
    add_link_options(${KERNEL_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/arch/${BUILD_ARCH}/${BUILD_ARCH_VER}/linker.ld)
endif()

add_executable(loader ${LOADER_SOURCES})
set_target_properties(loader PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES}")
target_link_libraries(loader PUBLIC c_kernel gcc)

# Compilation -----------------------------------------------------------------

# Do not allow builtin functions, libraries, or standard include. We're also freestanding.
target_compile_options(loader PRIVATE -Wall -ffreestanding -fno-builtin -nostdlib) 

# Installation ----------------------------------------------------------------
set_target_properties(
    loader
    PROPERTIES 
        OUTPUT_NAME "loader"
        SUFFIX ".elf"
)

install(TARGETS loader 
    DESTINATION boot/
)