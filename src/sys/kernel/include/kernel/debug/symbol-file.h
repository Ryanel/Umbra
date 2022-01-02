#pragma once

#include <kernel/types.h>

namespace kernel {
namespace debug {

struct symbol {
    uint64_t address;
    uint64_t str_off;
};

struct symbol_header {
    uint64_t num_symbols;
};

class symbol_server {
   public:
    void        init();
    const char* get_symbol(uint64_t ptr);

   private:
    bool        initialised = false;
    virt_addr_t start_addr;
    uint64_t    num_symbols;
};

extern symbol_server g_symbol_server;

}  // namespace debug
}  // namespace kernel