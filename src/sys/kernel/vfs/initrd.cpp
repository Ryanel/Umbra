#include <kernel/boot/boot_file.h>
#include <kernel/util/string.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>
#include <string.h>

int oct2bin(unsigned char* str, int size) {
    int            n = 0;
    unsigned char* c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

struct ustar_header {
    char const name[100];  // name
    uint64_t   file_mode;
    uint64_t   uid;
    uint64_t   gid;
    char       size_octal[12];
    char       last_modification[12];
    uint64_t   checksum;
    char       type;
    char const linked_file[100];
    char const ustar[6];
    char const ustar_ver[2];
};

namespace kernel {
namespace vfs {

void initrd_provider::init() {
    // First, get the initrd
    initrd_data = &kernel::g_bootfiles.files[1];  // TODO: A proper search
    auto* root  = kernel::vfs::g_vfs.get_root();  // The initial ramdisk directly overlays ontop of the root.

    // Now, comb throug the USTAR formatted initrd.
    virt_addr_t ptr = initrd_data->vaddr;
    while (true) {
        auto* header = (ustar_header*)(ptr);
        if (strcmp("ustar", (char const*)&header->ustar)) { break; }

        // Create the node
        size_t sz              = oct2bin((unsigned char*)header->size_octal, 11);
        auto*  node            = new vfs_node(root, this, vfs_type::file, sz);
        auto*  dat             = new fdata((virt_addr_t)header + 512);
        node->delegate_storage = (void*)dat;

        // Set the type
        switch (header->type) {
            case '\0':
            case '0': node->type = vfs_type::file; break;
            case '5': node->type = vfs_type::directory; break;
        }

        // Determine the parent and file name
        kernel::string filename = kernel::string((char*)&header->name);
        while (filename.find('/') != kernel::string::npos) {
            size_t delim_pos = filename.find('/');

            auto before_delim = filename.substr(0, delim_pos);

            // This means we have hit the last in a directory entry. Cut the last / off and go.
            if (delim_pos == (filename.size() - 1)) {
                filename = before_delim;
                break;
            }

            // Nope, find the parent.
            for (vfs_node_child* c = node->parent->children.front(); c != nullptr; c = c->m_next) {
                if (strcmp(c->node->name(), before_delim.data()) == 0) {
                    node->parent = c->node;
                    break;
                }
            }

            filename = filename.substr(delim_pos + 1);
        }

        memcpy(&node->name_buffer, filename.data(), 100);  // Copy the name
        node->parent->add_child(node);                     // Add this node to the VFS properly
        ptr += (((node->size + 511) / 512) + 1) * 512;     // Skip to the next file metadata block
    }
}

int initrd_provider::read(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
    if (node == nullptr) { return 1; }
    if (node->type != vfs_type::file) { return 2; }
    if ((offset + size) > node->size) { return 3; }

    fdata*   dat       = (fdata*)node->delegate_storage;
    uint8_t* file_data = (uint8_t*)(dat->location);

    for (size_t i = 0; i < size; i++) { buffer[i] = file_data[offset + i]; }
    return 0;
}

int initrd_provider::write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }

}  // namespace vfs
}  // namespace kernel
