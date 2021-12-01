# CMake Versioning...
cmake_minimum_required(VERSION 3.7...3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Project Definition
project(Umbra VERSION 1.0
                DESCRIPTION "A cool new OS"
                LANGUAGES C CXX ASM)

# Subdirectories to compile
add_subdirectory(sys) # The kernel
add_subdirectory(apps) # Binaries that end up on the filesystem and can be launched that are of system importance, such as init
add_subdirectory(usr) # User level utilities
add_subdirectory(lib) # Libraries that used by any applications, such as libc.
#add_subdirectory(src/boot) # Boot applications