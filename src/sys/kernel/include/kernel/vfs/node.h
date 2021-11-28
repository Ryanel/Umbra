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

struct vfs_node_child {
    vfs_node*       node;
    vfs_node_child* next;
};

struct vfs_node {
    char                                     name_buffer[VFS_MAX_FILENAME];
    vfs_delegate*                            delegate;
    vfs_node*                                parent;
    util::linked_list_inline<vfs_node_child> children;
    size_t                                   size;
    vfs_type                                 type;

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