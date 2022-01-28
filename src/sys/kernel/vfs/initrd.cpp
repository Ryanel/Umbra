#include <kernel/boot/boot_file.h>
#include <kernel/log.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/path.h>
#include <kernel/vfs/vfs.h>

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

void initrd_fs::init() {
    // First, get the initrd
    for (size_t i = 0; i < kernel::boot::g_bootfiles.numfiles; i++) {
        auto& file = kernel::boot::g_bootfiles.files[i];
        if (file.type != 1) { continue; }
        kernel::log::info("initrd", "using file %d\n", i);
        initrd_data = &kernel::boot::g_bootfiles.files[i];
        break;
    }

    // Now, comb through the USTAR formatted initrd.
    virt_addr_t ptr = initrd_data->vaddr;

    uint64_t inode = 2;

    m_root             = new node();
    m_root->m_inode_no = 1;
    m_root->m_fs       = this;
    m_root->m_delegate = nullptr;
    m_root->m_size     = 0;
    m_root->m_type     = node_type::directory;
    m_root->m_user_ptr = new fdata(0);

    m_nodes.push_back(m_root);

    while (true) {
        auto* header = (ustar_header*)(ptr);
        if (strcmp("ustar", (char const*)&header->ustar)) { break; }
        size_t sz  = oct2bin((unsigned char*)header->size_octal, 11);
        auto*  dat = new fdata((virt_addr_t)header + 512);

        node* new_node       = new node();
        new_node->m_inode_no = inode;
        new_node->m_fs       = this;
        new_node->m_delegate = nullptr;
        new_node->m_size     = sz;
        new_node->m_user_ptr = (void*)dat;
        // Set the type
        switch (header->type) {
            case '\0':
            case '0': new_node->m_type = node_type::file; break;
            case '5': new_node->m_type = node_type::directory; break;
        }

        size_t len      = strlen((char*)&header->name);
        auto   filename = std::string_view((char*)&header->name, len < 100 ? len : 100);

        kernel::vfs::path pth(filename);
        size_t            tokens = pth.tokens();

        filename = pth.get_index(tokens - 1);
        strncpy(new_node->m_name, filename.data(), filename.size());
        // Recursively find the node...
        node* parent = m_root;
        for (size_t i = 0; i < (tokens - 1); i++) {
            fdata* pfdata = ((fdata*)parent->m_user_ptr);
            // Search for the name...
            for (auto&& x : pfdata->m_children) {
                if (pth.get_index(i).compare(x->name()) == 0) { parent = x; }
            }
        }

        ((fdata*)parent->m_user_ptr)->m_children.push_back(new_node);

        m_nodes.push_back(new_node);

        ptr += (((sz + 511) / 512) + 1) * 512;  // Skip to the next file metadata block
    }
}

std::list<vfs::node*> initrd_fs::get_children(node* n) { return ((fdata*)n->m_user_ptr)->m_children; }

node* initrd_fs::get_node(uint64_t inode) {
    for (auto&& i : m_nodes) {
        if (i->m_inode_no == inode) { return i; }
    }

    return nullptr;
}

node*  initrd_fs::get_root() { return m_root; }
node*  initrd_fs::create(node* parent, std::string name) { return nullptr; }
bool   initrd_fs::remove(node* n) { return false; }
size_t initrd_fs::read(node* n, void* buffer, size_t cursor_pos, size_t num_bytes) { return -1; }
size_t initrd_fs::write(node* n, void* buffer, size_t cursor_pos, size_t num_bytes) { return -1; }

}  // namespace vfs
}  // namespace kernel
