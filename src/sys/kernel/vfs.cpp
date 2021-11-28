#include <kernel/mm/heap.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

namespace kernel {
namespace vfs {

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

        const char*  file_data   = "This is the contents of the test file!";
        size_t file_size   = node->size;
        size_t end_of_read = offset + size;

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

    auto* vfs_test_file           = new vfs_node();
    vfs_test_file->name_buffer[0] = 't';
    vfs_test_file->name_buffer[1] = 'e';
    vfs_test_file->name_buffer[2] = 's';
    vfs_test_file->name_buffer[3] = 't';
    vfs_test_file->name_buffer[4] = '_';
    vfs_test_file->name_buffer[5] = 'f';
    vfs_test_file->name_buffer[6] = 'i';
    vfs_test_file->name_buffer[7] = 'l';
    vfs_test_file->name_buffer[8] = 'e';

    vfs_test_file->type     = vfs_type::file;
    vfs_test_file->delegate = new test_delegate();
    vfs_test_file->parent   = nullptr;
    vfs_test_file->size     = 40;

    m_root->add_child(vfs_test_file);

    // Testing code
    printf("\n");

    print_tree(m_root);

    char buffer[255];
    int ret = vfs_test_file->delegate->read(vfs_test_file, 0, vfs_test_file->size, (uint8_t*)&buffer[0]);
    printf("Test read of test_file %d: %s\n",ret, (const char*)buffer);

    printf("\n");
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

    printf("%s (%d bytes) type: %s, delegate: %s\n", n->name(), n->size, ftype_string, n->delegate->delegate_name());

    for (vfs_node_child* c = n->children.front(); c != nullptr; c = c->next) { print_tree(c->node, depth + 1); }
}

}  // namespace vfs
}  // namespace kernel
