#pragma once

#include <kernel/log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <list>

#define VFS_MAX_FILENAME 128

namespace kernel {
namespace vfs {

class vfs_delegate;
class vfs_node;

enum class vfs_type : uint8_t { file = 1, directory = 2, device = 3 };
typedef uint32_t file_id_t;

struct file_descriptor {
    vfs_node*        m_node;
    file_descriptor* m_next;
    int              flags;
    file_id_t        m_id;
};

struct file_stats {
    size_t size;
};

struct vfs_node {
    char                 name_buffer[VFS_MAX_FILENAME];
    vfs_delegate*        delegate;
    vfs_node*            parent;
    void*                delegate_storage;
    std::list<vfs_node*> children;
    size_t               size;
    vfs_type             type;

    vfs_node() {}
    vfs_node(vfs_node* p, vfs_delegate* delegate, vfs_type type, size_t sz, const char* name = nullptr)
        : delegate(delegate), parent(p), size(sz), type(type) {
        if (p != nullptr) { p->add_child(this); }
        if (name != nullptr) { set_name(name); }
    }

    vfs_node(const vfs_node& n) {
        memcpy(name_buffer, n.name_buffer, VFS_MAX_FILENAME);
        delegate         = n.delegate;
        parent           = n.parent;
        delegate_storage = n.delegate_storage;
        size             = n.size;
        type             = n.type;
        kernel::log::error("node", "Copy\n");
    }

    char* name() const { return (char*)&name_buffer[0]; }
    void  set_name(char const* s) {
        for (size_t cnt = 0; cnt < VFS_MAX_FILENAME; cnt++) {
            name_buffer[cnt] = s[cnt];
            if (name_buffer[cnt] == '\0') { return; }
        }
    }

    void add_child(vfs_node* node) {
        node->parent = this;
        children.push_back(node);
    }
};

class vfs_delegate {
   public:
    virtual int         read(vfs_node* node, size_t offset, size_t size, uint8_t* buffer)  = 0;
    virtual int         write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) = 0;
    virtual char const* delegate_name()                                                    = 0;
};

}  // namespace vfs
}  // namespace kernel