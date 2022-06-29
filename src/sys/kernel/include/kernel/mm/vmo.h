#pragma once

#include <kernel/object.h>

namespace kernel {
namespace vm {

/// A VMO (virtual memory object) represents a single region of virtual memory. 
/// This region is mappable into multiple address spaces.

class vmo : public object {
     KOBJECT_CLASS_ID(4, "vm_object");
     uint64_t m_start;
     uint64_t m_size;
};

/// A VMC (virtual memory container) contains references to many VMOs.
/// A VMC is a reflection of the VM state in the range of managed address spaces.
class vmc : public object {
     KOBJECT_CLASS_ID(5, "vm_container");
};

};
};
