#pragma once

#include <kernel/config.h>
#include <kernel/vfs/types.h>
#include <stdint.h>

#include <string_view>

namespace kernel {
namespace vfs {

class filesystem;
class delegate;

/// A node is a in-memory representation of a file stored in a filesystem.
/// Nodes can be invalidated. Nodes are not forever, and can be deallocated and regenerated.
struct node {
    char m_name[128];

    uint64_t    m_inode_no;
    uint64_t    m_owner;
    uint64_t    m_mod_time;
    uint64_t    m_size;
    filesystem* m_fs;
    delegate*   m_delegate;
    void*       m_user_ptr;
    node_type   m_type;

    const char* name() const { return (const char*)&m_name; }
    void        set_name(std::string_view name) { 
        for (size_t i = 0; i < name.size(); i++)
        {
            m_name[i] = name.at(i);
        }
        
    }
};

}  // namespace vfs
}  // namespace kernel