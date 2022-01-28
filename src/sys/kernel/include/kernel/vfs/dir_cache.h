#pragma once

#include <stdint.h>

#include <string_view>
#include <unordered_map>

namespace kernel {
namespace vfs {

struct cache_node {
};

class node_cache {
   public:
    bool is_cached(std::string_view path);
    void invalidate();

   private:
    std::unordered_map<std::string, cache_node> m_node;
};

}  // namespace vfs
}  // namespace kernel