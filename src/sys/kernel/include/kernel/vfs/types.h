#pragma once

#include <stdint.h>

namespace kernel {
namespace vfs {

struct node;

enum class node_type : uint8_t { file, directory, mountpoint };

typedef uint32_t fd_id_t;

struct file_descriptor {
    node*            m_node;
    file_descriptor* m_next;
    fd_id_t          m_id;
    uint32_t         flags;
};

}  // namespace vfs
}  // namespace kernel