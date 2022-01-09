#include <kernel/boot/boot_file.h>
#include <kernel/panic.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

#include <string>

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
} __attribute__((packed));

namespace kernel {
namespace vfs {

void initrd_provider::init() {
    // First, get the initrd
    for (size_t i = 0; i < kernel::boot::g_bootfiles.numfiles; i++) {
        auto& file = kernel::boot::g_bootfiles.files[i];
        if (file.type != 1) { continue; }
        kernel::log::info("initrd", "using file %d\n", i);
        initrd_data = &kernel::boot::g_bootfiles.files[i];
        break;
    }

    auto* root = kernel::vfs::g_vfs.get_root();  // The initial ramdisk directly overlays ontop of the root.

    // Now, comb throug the USTAR formatted initrd.
    virt_addr_t ptr = initrd_data->vaddr;
    while (true) {
        auto* header = (ustar_header*)(ptr);
        if (strcmp("ustar", (char const*)&header->ustar)) { break; }

        // Create the node
        size_t sz              = oct2bin((unsigned char*)header->size_octal, 11);
        auto*  node            = new vfs_node(nullptr, this, vfs_type::file, sz);
        auto*  dat             = new fdata((virt_addr_t)header + 512);
        node->delegate_storage = (void*)dat;
        node->parent           = root;

        // Set the type
        switch (header->type) {
            case '\0':
            case '0': node->type = vfs_type::file; break;
            case '5': node->type = vfs_type::directory; break;
        }

        // Determine the parent and file name
        auto filename = std::string((char*)&header->name);
        while (filename.find('/') != std::string::npos) {
            size_t delim_pos = filename.find('/');

            auto before_delim = filename.substr(0, delim_pos);

            // This means we have hit the last in a directory entry. Cut the last / off and go.
            if (delim_pos == (filename.size() - 1)) {
                filename = before_delim;
                break;
            }

            // Nope, find the parent.
            for (auto&& i : node->parent->children) {
                if (strcmp(i->name(), before_delim.data()) == 0) {
                    node->parent = i;
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
    if (node->type != vfs_type::file) { return 2; }
    if ((offset + size) > node->size) { return -1; }

    assert(buffer != nullptr);
    assert(node != nullptr);

    fdata* dat = (fdata*)node->delegate_storage;
    assert(dat != nullptr);

    uint8_t* file_data = (uint8_t*)(dat->location);
    assert(file_data != nullptr);

    for (size_t i = 0; i < size; i++) { buffer[i] = file_data[offset + i]; }

    return 0;
}

int initrd_provider::write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }

}  // namespace vfs
}  // namespace kernel
