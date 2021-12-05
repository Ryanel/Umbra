#pragma once

#include <kernel/util/string.h>
#include <kernel/vfs/node.h>

namespace kernel {
namespace vfs {

class virtual_filesystem {
   public:
    void init();

    vfs_node* get_root() const { return m_root; }
    vfs_node* find(kernel::string path);

   private:
    vfs_node* m_root;
};

extern kernel::vfs::virtual_filesystem g_vfs;
}  // namespace vfs
}  // namespace kernel