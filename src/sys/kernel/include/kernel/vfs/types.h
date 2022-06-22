#pragma once

#include <stdint.h>
#include <kernel/object.h>

namespace kernel {
namespace vfs {

struct node;

enum class node_type : uint8_t { file, directory, mountpoint };

typedef uint32_t fd_id_t;

struct file_descriptor : public object {
    KOBJECT_CLASS_ID(3, "file_descriptor");
    node*   m_node;
    fd_id_t m_local_id;

    file_descriptor(node* n, fd_id_t id = -1) : m_node(n), m_local_id(id) {}
};

}  // namespace vfs
}  // namespace kernel