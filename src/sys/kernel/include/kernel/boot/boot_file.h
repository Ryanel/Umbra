#pragma once

#include <kernel/types.h>

#define BOOTFILE_MAX 10

namespace kernel {

struct boot_file {
    char const* name;
    virt_addr_t vaddr;
    phys_addr_t paddr;
    size_t      size;
};

class boot_file_container {
   public:
    boot_file files[BOOTFILE_MAX];
    int       numfiles = 0;

    void add(boot_file f) {
        files[numfiles] = f;
        numfiles++;
    }
};

extern boot_file_container g_bootfiles;

}  // namespace kernel