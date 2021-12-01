file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.s *.c *.s)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS *.h)
set (LINK_FLAGS )

add_link_options(-ffreestanding -fno-builtin -nostdlib -nostartfiles)

add_executable(test_program ${SOURCE_FILES})
target_compile_options(test_program PRIVATE -ffreestanding -fno-builtin -nostdlib -nostartfiles)
install(TARGETS test_program DESTINATION ../initrd/apps/)
