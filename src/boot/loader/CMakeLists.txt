# Loader ----------------------------------------------------------------------

# Source files
file(GLOB KERNEL_SOURCES_ROOT CONFIGURE_DEPENDS *.c *.cpp *.s include/loader/*.h)
file(GLOB KERNEL_SOURCES_ARCH CONFIGURE_DEPENDS ${BUILD_ARCH}/*.c ${BUILD_ARCH}/*.cpp ${BUILD_ARCH}/*.s includes/loader/${BUILD_ARCH}/*.h)
file(GLOB KERNEL_SOURCES_ARCHVER CONFIGURE_DEPENDS ${BUILD_ARCH}/${BUILD_ARCH_VER}/*.c ${BUILD_ARCH}/${BUILD_ARCH_VER}/*.cpp ${BUILD_ARCH}/${BUILD_ARCH_VER}/*.s includes/loader/${BUILD_ARCH}/${BUILD_ARCH_VER}/*.h)
file(GLOB_RECURSE KERNEL_SOURCES_BOARD CONFIGURE_DEPENDS ${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.c ${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.cpp ${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.s includes/loader/${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/*.h)

set(LOADER_SOURCES ${KERNEL_SOURCES_ROOT} ${KERNEL_SOURCES_ARCH} ${KERNEL_SOURCES_ARCHVER} ${KERNEL_SOURCES_BOARD})

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
set (LOADER_LINK_FLAGS -ffreestanding -fno-builtin -nostdlib -static)
if (${BUILD_HAS_BOARD})
    add_link_options(${LOADER_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_ARCH}/${BUILD_ARCH_VER}/${BUILD_BOARD}/linker.ld )
else()
    add_link_options(${LOADER_LINK_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_ARCH}/${BUILD_ARCH_VER}/linker.ld)
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