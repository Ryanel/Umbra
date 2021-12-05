#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

namespace kernel {
namespace vfs {

virtual_filesystem g_vfs;

/// The null delegate implements no file operations.
class null_delegate : public vfs_delegate {
   public:
    null_delegate() {}
    int         read(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    int         write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    char const* delegate_name() { return "null delegate"; }
};

void virtual_filesystem::init() {
    auto* vfs_root           = new vfs_node(nullptr, new null_delegate(), vfs_type::directory, 0);
    vfs_root->name_buffer[0] = '/';
    vfs_root->name_buffer[1] = 0;
    m_root                   = vfs_root;
}

vfs_node* virtual_filesystem::find(kernel::string path) {
    auto* directory = m_root;

    kernel::log::debug("vfs", "Finding %s\n", path.data());
    if (path.find('/') == 0) { path = path.substr(1); }

    while (path.find('/') != kernel::string::npos) {
        size_t delim_pos    = path.find('/');
        auto   before_delim = path.substr(0, delim_pos);

        // This is a directory, return it.
        if (delim_pos == (path.size() - 1)) { return directory; }

        // Check the prospective parents children to see if this directory is one of them.
        for (vfs_node_child* c = directory->children.front(); c != nullptr; c = c->m_next) {
            if (strcmp(c->node->name(), before_delim.data()) == 0) {
                directory = c->node;
                break;
            }
        }
        path = path.substr(delim_pos + 1);
    }

    // Search all the children of directory
    for (vfs_node_child* c = directory->children.front(); c != nullptr; c = c->m_next) {
        if (strcmp(c->node->name(), path.data()) == 0) { return c->node; }
    }

    // We didn't find it, return nullptr.
    return nullptr;
}

}  // namespace vfs
}  // namespace kernel
