#pragma once

#include <kernel/config.h>
#include <kernel/util/ref.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <list>

#ifdef ASSERT_HANDLE_TYPE
#include <kernel/log.h>
#include <kernel/panic.h>
#endif

// Type aliases
// -----------------------------------------------------------------------
typedef uint64_t k_id_t;
typedef uint32_t handle_t;
typedef uint32_t obj_type_id_t;

// Macros
// -----------------------------------------------------------------------

#define KERNEL_HANDLE_INVALID 0

#define KOBJECT_CLASS_ID(x, name)                                     \
    static constexpr obj_type_id_t object_type_id  = x;               \
    static constexpr const char*   object_name_str = name;            \
    virtual obj_type_id_t          object_type() const { return x; }; \
    virtual const char*            object_name() const { return name; }

// External objects
extern k_id_t s_next_kobject_id;

namespace kernel {

/// A kernel object.
class object : public ref_counted {
   public:
    object() { m_kid = s_next_kobject_id++; }
    virtual ~object() {}

    uint64_t              object_id() const { return m_kid; }
    virtual obj_type_id_t object_type() const = 0;
    virtual const char*   object_name() const = 0;

   private:
    k_id_t m_kid = 0;
};

/// A handle to a kernel object. Holds a reference to the object.
struct handle {
    ref<object> m_obj;  ///< Reference to the held object. This will always be valid unless ID is KERNEL_HANDLE_INVALID
    uint32_t m_owner;   ///< The owner ID of this handle. This is a task id. Task ID = 0 means kernel owns this handle.
    uint32_t m_rights;  ///< The rights mask for this handle
    uint32_t m_id;      ///< The local handle ID

    /// An empty handle is invalid, but these should normally not be created.
    handle() {
        m_obj    = ref<object>(nullptr);
        m_owner  = 0;
        m_rights = 0;
        m_id     = KERNEL_HANDLE_INVALID;
    }

    ~handle() {
        m_id     = 0;
        m_rights = 0;
        m_owner  = 0;
        m_obj.reset(nullptr);
    }

    /// Create a handle to the kernel object
    handle(ref<object> obj, uint32_t owner, uint32_t rights, uint32_t id)
        : m_obj(obj), m_owner(owner), m_rights(rights), m_id(id) {}

    /// Handles can have rights revoked. Accepts a bitmask of the rights to revoke.
    /// Rights cannot be re-added to a handle.
    /// Pending operations may or may not stop. This function simply modifies the rights of this handle.
    void revoke_rights(uint32_t mask) {
        m_rights = m_rights & (~mask);
    }

    template <typename U>
    bool is() {
        if (m_id == KERNEL_HANDLE_INVALID) { return false; }
        return m_obj->object_type() == U::object_type_id;
    }

    /// Casts the held object to
    template <typename U>
    ref<U> as() {
        if (m_id == KERNEL_HANDLE_INVALID) {
            kernel::log::error("handle", "Attempted to reference an invalid handle\n");
            kernel::log::error("handle", "Tried to convert %s into %s.\n", m_obj->object_name(), U::object_name_str);
            kernel::log::error("handle", "Handle ID: %d, Rights: 0x%08x, owner %d\n", m_id, m_rights, m_owner);
            assert("Attempted to reference an invalid handle");
        } else if (m_obj->object_type() != U::object_type_id) {
            kernel::log::error("handle", "Invalid type conversion within handle!\n");
            kernel::log::error("handle", "Tried to convert %s into %s.\n", m_obj->object_name(), U::object_name_str);
            kernel::log::error("handle", "Handle ID: %d, Rights: 0x%08x, owner %d\n", m_id, m_rights, m_owner);
            assert("Invalid handle created");
        }

        return ref<U>::downcast(m_obj);
    }
};

/// Holds a collection of handles.
class handle_registry {
   public:
    handle_registry() {}
    ~handle_registry() {
        // TODO: Destroy each handle in the list
    }

    /// Creates a handle to an object. The owner is the task ID associated with this handle. Rights must be set right away.
    handle* create(ref<object> obj, uint32_t owner, uint32_t rights, uint32_t id = 0) {
        if (id == 0) { id = next_id++; }
        auto* r = new handle(obj, owner, rights, id);
        m_handles.push_back(r);
        return r;
    }

    /// Duplicates a handle. The new handle has the same owner and rights, but a different ID.
    handle* duplicate(handle* to_duplicate, uint32_t owner = -1) {
        uint32_t new_owner = owner == -1 ? to_duplicate->m_owner : owner;
        auto* r = new handle(to_duplicate->m_obj, new_owner, to_duplicate->m_rights, next_id++);
        m_handles.push_back(r);
        return r;
    }

    handle* duplicate_and_transfer(handle* to_duplicate, handle_registry* newRegistry, uint32_t owner = -1) {
        uint32_t new_owner = owner == -1 ? to_duplicate->m_owner : owner;
        handle* newhnd = newRegistry->create(to_duplicate->m_obj, new_owner, to_duplicate->m_rights);
        return newhnd;
    }    

    void remove(handle* hnd) {
        m_handles.erase(std::find(m_handles.begin(), m_handles.end(), hnd));
        delete hnd;
    }

    handle* get(uint32_t hid) {
        for (auto&& i : m_handles) {
            if (i->m_id == hid) { return i; }
        }
        return nullptr;
    }

    // Transfer
    uint32_t transfer(handle_registry* r, handle* hnd) {
        m_handles.erase(std::find(m_handles.begin(), m_handles.end(), hnd));
        hnd->m_id = 0;
        return r->recieve_transfer(hnd);
    }

    void debug() {
        for (auto &&i : m_handles)
        {
            const char* obj_type = "?????";

            if (i->m_obj != nullptr) {
                obj_type = i->m_obj->object_name();
            }

            log::trace("hnd", "%2d: %s\n", i->m_id, obj_type);
        }
    }

   private:
    uint32_t recieve_transfer(handle* hnd) {
        hnd->m_id = next_id++;
        m_handles.push_back(hnd);
        return hnd->m_id;
    }

   private:
    std::list<handle*> m_handles;
    uint32_t           next_id = 1;
};

}  // namespace kernel