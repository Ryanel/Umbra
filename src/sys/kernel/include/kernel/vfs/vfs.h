#pragma once

#include <kernel/vfs/node.h>
#include <kernel/vfs/types.h>

#include <list>
#include <string>
#include <string_view>


#define VFS_OPEN_FLAG_CREATE 1

namespace kernel {
namespace vfs {

struct node;
class filesystem;

class virtual_filesystem {
   public:
    void init();

    /// Finds the node at the specified path.
    /// The path is always assumed to be in the root
    /// Paths should take the form of /path/name/here/targetname . Does not handle relative paths (.. , . , ~).
    /// Userspace must convert relative paths to absolute paths.
    node* find(std::string_view path);
    bool  mount(std::string_view path, filesystem* fs);

    fd_id_t open_file(std::string_view path, int flags);
    size_t  read(fd_id_t fd, uint8_t* buf, size_t count);
    size_t  write(fd_id_t fd, uint8_t* buf, size_t count);
    // file_stats fstat(fd_id_t fd);

   private:
    struct mountpoint {
        std::string m_path;
        filesystem* m_fs;
    };

    std::list<mountpoint> m_mountpoints;
    mountpoint*           get_mountpoint(std::string_view path);

    std::list<file_descriptor*> open_files;
    fd_id_t                     next_descriptor_id = 0;
    file_descriptor*            taskfd_to_fd(fd_id_t id);
};

extern kernel::vfs::virtual_filesystem g_vfs;
}  // namespace vfs
}  // namespace kernel