#pragma once

#include <kernel/vfs/filesystem.h>

#include <list>

namespace kernel {
namespace vfs {

/// An in-memory filesystem.
class tmp_fs : public filesystem {
   public:
    void init();

    virtual std::list<node*> get_children(node* node);
    virtual node*            get_node(uint64_t inode);
    virtual node*            get_root();
    virtual node*            create(node* parent, std::string_view name);
    virtual bool             remove(node* n);
    virtual size_t           read(node* n, void* buffer, size_t cursor_pos, size_t num_bytes);
    virtual size_t           write(node* n, void* buffer, size_t cursor_pos, size_t num_bytes);

    char const* name() { return "tmpfs"; }

   private:
    std::list<node*> m_nodes;
    node*            m_root;
    size_t           m_next_inode = 2;

    struct fdata {
        uint8_t*         memory;
        size_t           size;
        std::list<node*> m_children;
    };
};

}  // namespace vfs
}  // namespace kernel