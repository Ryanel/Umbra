#pragma once

#include <kernel/vfs/node.h>
#include <kernel/string.h>

namespace kernel {
namespace vfs {
class virtual_filesystem {
   public:
    void      init();
    void      print_tree(vfs_node* n, int depth = 0);
    void      debug();
    vfs_node* get_root() const { return m_root; }

    vfs_node* find(kernel::string path);
   private:
    vfs_node* m_root;
};

extern kernel::vfs::virtual_filesystem g_vfs;
}  // namespace vfs
}  // namespace kernel