# Set needed info about the system
SET (BUILD_ARCH x86)
SET (BUILD_ARCH_VER i686)
SET (BUILD_BOARD i686)
SET (BUILD_HAS_BOARD true)
SET (BUILD_BITWIDTH 32)
set(triple i686-elf-none)

# Configure CMake
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${BUILD_ARCH})

set(CMAKE_SYSROOT ../sysroot/)
set(CMAKE_STAGING_PREFIX ../sysroot/)

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_C_COMPILER_TARGET ${triple})
#set(CMAKE_CXX_COMPILER_TARGET ${triple})
#SET(ASM_OPTIONS "-x assembler-with-cpp -target ${triple}")

set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_AR i686-elf-ar)

set(KERNEL_ADDITIONAL_LINK_FLAGS "")

set(CMAKE_C_FLAGS "-march=i686 -mtune=generic")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_DEBUG "-Og -g -gdwarf-3 -gstrict-dwarf")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Tell CMake to not try and compile test executables.

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set some useful flags for the platform...
SET(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}" )