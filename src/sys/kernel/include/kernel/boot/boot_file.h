#pragma once

#include <kernel/config.h>
#include <kernel/types.h>

namespace kernel {
namespace boot {
struct boot_file {
    uintptr_t type; // 0x0 = initrd, 0x1 = symbols
    virt_addr_t vaddr;
    phys_addr_t paddr;
    size_t      size;
};

class boot_file_container {
   public:
    void add(boot_file f) { files[numfiles++] = f; }

   public:
    boot_file files[BOOTFILE_MAX];
    int       numfiles = 0;
};

extern boot_file_container g_bootfiles;

}  // namespace boot
}  // namespace kernel