#pragma once

#include <kernel/vfs/filesystem.h>

#include <list>
namespace kernel {

// Prototypes
namespace boot {
struct boot_file;
}

namespace vfs {

class initrd_fs : public filesystem {
   public:
    void init();

    virtual std::list<node*> get_children(node* node);
    virtual node*            get_node(uint64_t inode);
    virtual node*            get_root();
    virtual node*            create(node* parent, std::string name);
    virtual bool             remove(node* n);
    virtual size_t           read(node* n, void* buffer, size_t cursor_pos, size_t num_bytes);
    virtual size_t           write(node* n, void* buffer, size_t cursor_pos, size_t num_bytes);

    char const* name() { return "initrd"; }

   private:
    boot::boot_file* initrd_data;
    std::list<node*> m_nodes;
    node*            m_root;
    struct fdata {
        uintptr_t        location;
        std::list<node*> m_children;
        fdata(uintptr_t loc) : location(loc) {}
    };
};

}  // namespace vfs
}  // namespace kernel