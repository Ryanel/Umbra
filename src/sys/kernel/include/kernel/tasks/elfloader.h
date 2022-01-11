#pragma once

#include <string>

namespace kernel {
namespace tasks {
class elf_loader {
   public:
    static void load_elf(const char * fpath);
};
}  // namespace tasks
}  // namespace kernel
