#pragma once

#include <kernel/util/linked_list.h>
#include <stddef.h>
#include <stdint.h>

#define VFS_MAX_FILENAME 128

namespace kernel {
namespace vfs {

class vfs_delegate;
class vfs_node;

enum class vfs_type : uint8_t { file = 1, directory = 2 };
typedef uint32_t file_id_t;

struct vfs_node_child {
    vfs_node*       node;
    vfs_node_child* m_next;
};

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
    char                                     name_buffer[VFS_MAX_FILENAME];
    vfs_delegate*                            delegate;
    vfs_node*                                parent;
    void*                                    delegate_storage;
    util::linked_list_inline<vfs_node_child> children;
    size_t                                   size;
    vfs_type                                 type;

    vfs_node() {}
    vfs_node(vfs_node* parent, vfs_delegate* delegate, vfs_type type, size_t sz) : parent(parent), delegate(delegate), type(type), size(sz) {
        if (parent != nullptr) { parent->add_child(this); }
    }

    char* name() const { return (char*)&name_buffer[0]; }

    void add_child(vfs_node* node) {
        auto* llnode = new vfs_node_child();
        node->parent = this;
        llnode->node = node;
        children.push_back(llnode);
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