file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.s *.c *.h)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS *.h)

include_directories(./include)

#add_library(c STATIC ${SOURCE_FILES})
add_library(c_kernel STATIC ${SOURCE_FILES})

#target_include_directories(c PUBLIC include)
target_include_directories(c_kernel PUBLIC include)

#target_compile_options(c PRIVATE  -ffreestanding -fno-builtin -nostdlib)
target_compile_options(c_kernel PRIVATE -D __is_libk -ffreestanding -fno-builtin -nostdlib) 

install(TARGETS c_kernel LIBRARY )
