#pragma once

#include <stdint.h>

#include <utility>

namespace kernel {

/// Holds a reference to a reference counted object of type T. The object is deleted if it's reference count drops to 0.
/// The object should implement ref_counted.
template <typename T>
class ref {
   public:
    // Null constructor
    constexpr ref() : m_ptr(nullptr) {}
    // Nullptr constructor
    constexpr ref(decltype(nullptr)) : ref() {}
    // Explict type constructor
    explicit ref(T* ptr) : m_ptr(ptr) {
        if (ptr != nullptr) { ptr->ref_inc(); }
    }

    // Implicit upcast from U to T
    template <typename U>
    ref(const ref<U>& r) : ref(r.m_ptr) {}

    // Explicit downcast constructor
    template <typename BT>
    ref(T* ptr, ref<BT>&& base) : m_ptr(ptr) {
        if (static_cast<BT*>(m_ptr) != base.m_ptr) { throw; };
        base.m_ptr = nullptr;
    }

    // Copy Constructor
    ref(const ref& o) : ref(o.m_ptr) {}

    // Move Constructor
    ref(ref&& o) : m_ptr(o.m_ptr) { o.m_ptr = nullptr; }

    // Copy Assignment
    ref& operator=(const ref& r) {
        // Ref first so self-assignments work.
        if (r.m_ptr) { r.m_ptr->ref_inc(); }
        T* old = m_ptr;
        m_ptr  = r.m_ptr;
        if (old && old->ref_dec()) { destroy(old); }
        return *this;
    }

    // Move assignment
    ref& operator=(ref&& o) {
        ref(std::move(o)).swap(*this);
        return *this;
    }

    // Destructor
    ~ref() {
        if (m_ptr && m_ptr->ref_dec()) { destroy(m_ptr); }
    }

    // Explicit downcast
    template <typename BaseRef>
    static ref downcast(BaseRef base) {
        if (base != nullptr) return ref<T>(static_cast<T*>(base.m_ptr));
        return nullptr;
    }

    /// Destorys the pointer
    void destroy(T* ptr) {
        if (ptr != nullptr) { delete ptr; }
    }

    /// Swaps pointers with another ref
    void swap(ref& o) {
        T* p    = m_ptr;
        m_ptr   = o.m_ptr;
        o.m_ptr = p;
    }

    /// Resets this ref
    void reset(T* ptr = nullptr) { ref(ptr).swap(*this); }
    /// Duplicates this ref
    ref<T> duplicate() { return ref<T>(m_ptr); }

    // Access Operators
    T* get() const { return m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    // Comparison Operators
    explicit operator bool() const { return m_ptr != nullptr; }
    bool     operator==(decltype(nullptr)) const { return (m_ptr == nullptr); }
    bool     operator!=(decltype(nullptr)) const { return (m_ptr != nullptr); }

    bool operator==(const ref<T>& o) const { return m_ptr == o.m_ptr; }
    bool operator!=(const ref<T>& o) const { return m_ptr != o.m_ptr; }

   private:
    template <typename U>
    friend class ref;

    T* m_ptr;
};

/// Creates a reference counted object
template <typename T>
inline ref<T> make_ref(T* ptr) {
    return ref<T>(ptr);
}

/// Inheritble class that allows a class to be reference counted.
class ref_counted {
   public:
    void ref_inc() { m_refs++; }
    bool ref_dec() {
        m_refs--;
        return m_refs == 0;
    }

    uintptr_t ref_count() const { return m_refs; }

   private:
    uintptr_t m_refs = 0;
};

}  // namespace kernel