#pragma once

#include <stdint.h>

#include <list>
#include <string>

namespace kernel {
namespace vfs {

struct node;

///
/// A delegate can "override" and intercept operations on a file. Typically used for device files.
///
class delegate {
   public:
    virtual size_t           read(node* n, void* buffer, size_t cursor_pos, size_t num_bytes)  = 0;
    virtual size_t           write(node* n, void* buffer, size_t cursor_pos, size_t num_bytes) = 0;
};

}  // namespace vfs
}  // namespace kernel