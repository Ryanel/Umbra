#pragma once

#include <kernel/types.h>
#include <kernel/config.h>

namespace kernel {

struct boot_file {
    char const* name;
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

}  // namespace kernel