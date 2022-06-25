#pragma once

#include <kernel/object.h>
#include <kernel/vfs/node.h>
#include <kernel/vfs/types.h>

#include <list>
#include <string>
#include <string_view>

#define VFS_OPEN_FLAG_CREATE_DIRECTORY 1
#define VFS_OPEN_FLAG_CREATE_FILE 2

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

    handle* open_file(std::string_view path, int flags);
    handle* create_device(std::string_view path, std::string_view name, delegate* delegate, node_type type);
    size_t  read(handle* hnd, uint8_t* buf, size_t count);
    size_t  write(handle* hnd, uint8_t* buf, size_t count);
    // file_stats fstat(fd_id_t fd);

   private:
    struct mountpoint {
        std::string m_path;
        filesystem* m_fs;
    };

    std::list<mountpoint*> m_mountpoints;
    handle_registry       m_open_files;
    mountpoint*           get_mountpoint(std::string_view path);
};

extern kernel::vfs::virtual_filesystem g_vfs;
}  // namespace vfs
}  // namespace kernel