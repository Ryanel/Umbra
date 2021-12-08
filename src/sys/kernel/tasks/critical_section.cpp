#include <kernel/tasks/critical_section.h>
#include <kernel/tasks/scheduler.h>
using namespace kernel::tasks;

critical_section::critical_section() { scheduler::lock(); }
critical_section::~critical_section() { scheduler::unlock(); }
