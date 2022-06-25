#include <kernel/log.h>
#include <kernel/vfs/tmpfs.h>
#include <kernel/vfs/path.h>
#include <kernel/vfs/vfs.h>

#include <string>

namespace kernel {
namespace vfs {

void tmp_fs::init() {
    m_root             = new node();
    m_root->m_inode_no = 1;
    m_root->m_fs       = this;
    m_root->m_delegate = nullptr;
    m_root->m_size     = 0;
    m_root->m_type     = node_type::directory;
    m_root->m_user_ptr = new fdata();

    m_nodes.push_back(m_root);
}

std::list<vfs::node*> tmp_fs::get_children(node* n) { return ((fdata*)n->m_user_ptr)->m_children; }

node* tmp_fs::get_node(uint64_t inode) {
    for (auto&& i : m_nodes) {
        if (i->m_inode_no == inode) { return i; }
    }

    return nullptr;
}

node*  tmp_fs::get_root() { return m_root; }
node*  tmp_fs::create(node* parent, std::string_view name) { 
    node* n = new node();
    n->m_inode_no = m_next_inode;
    n->m_fs = this;
    n->m_delegate = this;
    n->m_size = 0;
    n->m_type = node_type::file;
    n->m_user_ptr = new fdata();
    n->set_name(name);
    
    fdata* parentdata = (fdata*)parent->m_user_ptr;
    parentdata->m_children.push_back(n);
    m_nodes.push_back(n);
    return n;
}
bool   tmp_fs::remove(node* n) { return false; }

size_t tmp_fs::read(node* n, void* buffer, size_t cursor_pos, size_t num_bytes) { 
    if (n == nullptr) { return -1;}
    if (n->m_delegate != this) { return -1; }
    if (buffer == nullptr) { return -1; }
    if (n->m_type != node_type::file) { return -1; }
    if (cursor_pos > n->m_size) { return -1; }

    return -1; // TODO: Unimplemented.
}
size_t tmp_fs::write(node* n, void* buffer, size_t cursor_pos, size_t num_bytes) {
    if (n == nullptr) { return -1;}
    if (n->m_delegate != this) { return -1; }
    if (buffer == nullptr) { return -1; }
    if (n->m_type != node_type::file) { return -1; }
    if (cursor_pos > n->m_size) { return -1; }

    return -1; 
}

}  // namespace vfs
}  // namespace kernel
