#pragma once

#include <kernel/util/linked_list.h>
#include <kernel/util/string.h>
#include <kernel/vfs/node.h>

#define FILE_CREATE    0x1
#define FILE_TEMPORARY 0x2

#define VFS_FIND_FILE   0x1
#define VFS_FIND_PARENT 0x2

namespace kernel {
namespace vfs {

class virtual_filesystem {
   public:
    void init();

    vfs_node* get_root() const { return m_root; }
    vfs_node* find(kernel::string path, int flags = VFS_FIND_FILE);

    file_id_t  open_file(kernel::string path, int flags);
    size_t     read(file_id_t fd, uint8_t* buf, size_t count);
    size_t     write(file_id_t fd, uint8_t* buf, size_t count);
    file_stats fstat(file_id_t fd);

    file_descriptor* taskfd_to_fd(file_id_t);

   private:
    vfs_node*                                 m_root;
    util::linked_list_inline<file_descriptor> open_files;
    file_id_t                                 next_descriptor_id = 0;
};

extern kernel::vfs::virtual_filesystem g_vfs;
}  // namespace vfs
}  // namespace kernel