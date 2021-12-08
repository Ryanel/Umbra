#pragma once

#include <kernel/vfs/node.h>

namespace kernel {

// Prototypes
namespace boot {
struct boot_file;
}

namespace vfs {

class initrd_provider : public vfs_delegate {
   public:
    initrd_provider() {}
    boot::boot_file* initrd_data;

    void init();
    int  read(vfs_node* node, size_t offset, size_t size, uint8_t* buffer);
    int  write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer);

    char const* delegate_name() { return "initrd"; }

   private:
    struct fdata {
        unsigned int location;
        fdata(unsigned int loc) : location(loc) {}
    };
};

}  // namespace vfs
}  // namespace kernel