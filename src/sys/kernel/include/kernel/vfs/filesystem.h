#pragma once

#include <stdint.h>

#include <list>
#include <string>

namespace kernel {
namespace vfs {

struct node;

class filesystem {
   public:
    virtual std::list<node*> get_children(node* node)                                          = 0;
    virtual node*            get_node(uint64_t inode)                                          = 0;
    virtual node*            get_root()                                                        = 0;
    virtual node*            create(node* parent, std::string name)                            = 0;
    virtual bool             remove(node* n)                                                   = 0;
    virtual size_t           read(node* n, void* buffer, size_t cursor_pos, size_t num_bytes)  = 0;
    virtual size_t           write(node* n, void* buffer, size_t cursor_pos, size_t num_bytes) = 0;
    virtual const char*      name()                                                            = 0;
    virtual ~filesystem() {}

    const size_t unsupported_operation = -1;
    const size_t error                 = -2;
};

}  // namespace vfs
}  // namespace kernel