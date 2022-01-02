#include <kernel/boot/boot_file.h>
#include <kernel/debug/symbol-file.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <string.h>
namespace kernel {
namespace debug {

symbol_server g_symbol_server;

void symbol_server::init() {
    for (size_t i = 0; i < kernel::boot::g_bootfiles.numfiles; i++) {
        auto& file = kernel::boot::g_bootfiles.files[i];
        if (file.type != 0) { continue; }
        kernel::log::info("symserver", "using file %d\n", i);
        start_addr = file.vaddr;
        break;
    }

    num_symbols = ((symbol_header*)start_addr)->num_symbols;

    log::info("symserver", "symbols: %l\n", num_symbols);

    initialised = true;
}

const char* symbol_server::get_symbol(uint64_t ptr) {
    if (initialised) {
        uint64_t next_address = start_addr + 8;
        symbol*  psym         = (symbol*)(next_address);
        for (size_t i = 0; i < num_symbols; i++) {
            symbol* sym = (symbol*)(next_address);
            if (ptr < sym->address) { return (const char*)(start_addr + psym->str_off); }
            next_address += sizeof(symbol);
            psym = sym;
        }
        return "<not found>";
    } else {
        return "<symbols not loaded>";
    }
}

}  // namespace debug
}  // namespace kernel