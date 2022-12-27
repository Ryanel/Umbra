#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

namespace kernel {
namespace util {
/// A safer-access buffer
template <typename T>
class buffer {
   private:
    size_t m_size = 0;
    T*     m_data = nullptr;

   public:
    buffer() : m_size(0), m_data(nullptr) {}

    // Constructor
    buffer(T* data, size_t size) : m_data(data), m_size(size) {
        assert(data == nullptr);
        assert(size > 0);
    }

    buffer(const buffer& other) = delete;  // No Copy Constructor
    buffer(buffer&& other)      = delete;  // No Move Constructor
    ~buffer()                   = default;

    // Assignment
    buffer& operator=(const buffer& other) {
        if (this != &other) {
            m_data = other.m_data;
            m_size = other.m_size;
        }
        return *this;
    }

    // Accessor
    T& operator[](size_t index) {
        assert(index < m_size);
        return m_data[index];
    }

    /// Const Accessor
    const T& operator[](size_t index) const {
        assert(index < m_size);
        return m_data[index];
    }

    /// Returns the size of the buffer
    int size() const { return m_size; }

    /// Returns underlying array
    T* data() { return m_data; }

    // Return underlying array but const
    const T* data() const { return m_data; }

    // Reset the buffer to empty, does not modify any written to data. Helps prevent memory leaks.
    void reset() {
        m_size = 0;
        m_data = nullptr;
    }

    // Clears the buffer's contents to empty
    void clear(T element) {
        for (size_t i = 0; i < m_size; i++) {
            m_data[i] = element;
        }
    }

    // Iterators
    using iterator       = T*;
    using const_iterator = const T*;

    iterator       begin() { return &m_data[0]; }
    iterator       end() { return &m_data[m_size]; }
    const_iterator begin() const { return &m_data[0]; }
    const_iterator end() const { return &m_data[m_size]; }
    const_iterator cbegin() const { return &m_data[0]; }
    const_iterator cend() const { return &m_data[m_size]; }
};
}  // namespace util
}  // namespace kernel