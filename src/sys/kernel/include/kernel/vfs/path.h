#pragma once

#pragma once

#include <kernel/vfs/node.h>
#include <kernel/vfs/types.h>

#include <list>
#include <string>
#include <string_view>

namespace kernel {
namespace vfs {

class path {
   public:
    path(std::string_view name) : m_path(name) {}

    std::string_view get_index(size_t idx) {
        size_t path_ptr = 0;
        if (idx == 0 && m_path.at(0) == '/') { return std::string_view(m_path.data(), 1); }

        for (size_t i = 0; i < (idx + 1); i++) {
            if (idx == 0 && m_path.at(0) == '/' && i == 0) {
                path_ptr = 1;
                continue;
            }

            size_t next_delim = m_path.find('/', path_ptr);

            if (idx == i) { return m_path.substr(path_ptr, next_delim); }
            path_ptr = next_delim + 1;
        }
        return m_path;
    }

    size_t tokens() {
        size_t path_ptr = 0;
        size_t count    = 0;
        while (path_ptr < m_path.size()) {
            size_t next_delim = m_path.find('/', path_ptr);
            count++;

            if (next_delim == (m_path.size() - 1)) { return count; }
            if (next_delim == m_path.npos) { return count; }

            path_ptr = next_delim + 1;
        }
        return count;
    }

    bool isFile() const { return m_path.rfind('/') == (m_path.size() - 1); }

   private:
    std::string_view m_path;
};

}  // namespace vfs
}  // namespace kernel