#pragma once

#include <kernel/x86/cpu.h>

#define MAX_CPUS 16

namespace kernel {

extern cpu_data_t g_cpu_data[MAX_CPUS];

uintptr_t cpu_get_stackptr();

}  // namespace kernel