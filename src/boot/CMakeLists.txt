project (Loader LANGUAGES C CXX ASM)

cmake_minimum_required(VERSION 3.7...3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

add_subdirectory(../lib/c/ libc) # Explicitly only add in libc

if (BUILD_ARCH STREQUAL "x86") 
    add_subdirectory(loader)
endif()

