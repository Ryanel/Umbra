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

class test_delegate : public vfs_delegate {
   public:
    test_delegate() {}
    int read(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        if (node == nullptr) { return 1; }
        if (node->type != vfs_type::file) { return 2; }

        const char* file_data   = "This is the contents of the test file!";
        size_t      file_size   = node->size;
        size_t      end_of_read = offset + size;

        if (end_of_read > file_size) { return 3; }

        // Horrid, replace
        for (size_t i = 0; i < size; i++) { buffer[i] = file_data[offset + i]; }
        return 0;
    }

    int         write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    char const* delegate_name() { return "test delegate"; }
};

void virtual_filesystem::init() {
    auto* vfs_root           = new vfs_node();
    vfs_root->name_buffer[0] = '/';
    vfs_root->type           = vfs_type::directory;
    vfs_root->delegate       = new null_delegate();
    vfs_root->parent         = nullptr;
    vfs_root->size           = 0;
    m_root                   = vfs_root;
}

void virtual_filesystem::print_tree(vfs_node* n, int depth) {
    if (n == nullptr) { return; }
    for (int i = 0; i < depth; i++) { printf("    "); }

    char const* ftype_string;
    switch (n->type) {
        case vfs_type::file:
            ftype_string = "file";
            break;
        case vfs_type::directory:
            ftype_string = "dir";
            break;
        default:
            ftype_string = "???";
            break;
    }

    printf("%-20s (%3d bytes) type: %5s, delegate: %s\n", n->name(), n->size, ftype_string, n->delegate->delegate_name());

    for (vfs_node_child* c = n->children.front(); c != nullptr; c = c->next) { print_tree(c->node, depth + 1); }
}

vfs_node* virtual_filesystem::find(kernel::string path) {
    auto* directory = m_root;

    if (path.find('/') == 0) { path = path.substr(1); }

    kernel::log::debug("vfs", "Find %s\n", path.data());

    while (path.find('/') != kernel::string::npos) {
        size_t delim_pos    = path.find('/');
        auto   before_delim = path.substr(0, delim_pos);

        // This means we have hit the last in a directory entry. Cut the last / off and return
        if (delim_pos == (path.size() - 1)) {
            kernel::log::trace("vfs", "Returning directory\n");
            return directory;
        }

        // Nope, find the parent.
        for (vfs_node_child* c = directory->children.front(); c != nullptr; c = c->next) {
            kernel::log::trace("vfs", "Compare %s to %s (path: %s)\n", c->node->name(), before_delim.data(), path.data());
            if (strcmp(c->node->name(), before_delim.data()) == 0) {
                kernel::log::trace("vfs", "Found %s\n", c->node->name());
                directory = c->node;
                break;
            }
        }

        path = path.substr(delim_pos + 1);
        kernel::log::trace("vfs", "Find %s\n", path.data());
    }

    // Search all the children of directory
    for (vfs_node_child* c = directory->children.front(); c != nullptr; c = c->next) {
        kernel::log::trace("vfs", "Compare %s to %s (path: %s)\n", c->node->name(), path.data(), path.data());
        if (strcmp(c->node->name(), path.data()) == 0) {
            kernel::log::trace("vfs", "Returning %s\n", c->node->name());
            return c->node;
        }
    }

    return nullptr;
}

void virtual_filesystem::debug() {
    printf("\n");
    print_tree(m_root);
    printf("\n");
}

}  // namespace vfs
}  // namespace kernel
