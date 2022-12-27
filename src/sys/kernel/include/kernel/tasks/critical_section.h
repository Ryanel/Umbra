#pragma once
#include <kernel/tasks/scheduler.h>

namespace kernel {
namespace tasks {

struct critical_section {

    critical_section() { scheduler::lock(); }
    ~critical_section() { scheduler::unlock(); }

};

};  // namespace tasks
};  // namespace kernel