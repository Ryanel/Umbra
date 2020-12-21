#pragma once

namespace kernel {

/// This holds basic information about the system and processors.
class system {
    void register_new_core(int core_number, int num_hw_threads);
    void register_new_thread();
   private:
    int thread_count_total;
    int core_count_total;
};
}  // namespace kernel