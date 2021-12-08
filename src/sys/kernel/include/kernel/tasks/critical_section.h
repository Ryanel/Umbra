#pragma once

namespace kernel {
namespace tasks {

struct critical_section {
    critical_section();
    ~critical_section();
};

};  // namespace tasks
};  // namespace kernel