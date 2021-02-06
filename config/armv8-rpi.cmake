# Set needed info about the system
SET (BUILD_ARCH arm)
SET (BUILD_ARCH_VER v8)
SET (BUILD_BOARD raspi)
SET (BUILD_HAS_BOARD true)
SET (BUILD_BITWIDTH 64)

set(triple aarch64-elf)

# Configure Cake
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot/)
set(CMAKE_STAGING_PREFIX ${CMAKE_BINARY_DIR}/sysroot/)

set(CMAKE_C_COMPILER aarch64-elf-gcc)
set(CMAKE_CXX_COMPILER aarch64-elf-g++)

#set(CMAKE_C_COMPILER i686)
#set(CMAKE_C_COMPILER_TARGET ${triple})
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(CMAKE_C_FLAGS_RELEASE "-O0")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -gdwarf-3 -gstrict-dwarf")
#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_DEBUG})

set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -gdwarf-3 -gstrict-dwarf")
#set(CMAKE_CXX_FLAGS "")

set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Tell CMake to not try and compile test executables.

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set some useful flags for the platform...
SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}" )
