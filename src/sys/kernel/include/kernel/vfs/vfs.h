#pragma once

#include <kernel/vfs/node.h>

namespace kernel {
namespace vfs {
class virtual_filesystem {
   public:
    void init();

    void print_tree(vfs_node* n, int depth=0);
   private:
    vfs_node* m_root;
};
}  // namespace vfs
}  // namespace kernel