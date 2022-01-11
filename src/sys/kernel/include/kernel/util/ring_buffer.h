#pragma once

#include <stdint.h>

namespace kernel {
namespace util {
template <class T>
class ring_buffer {
   public:
    explicit ring_buffer(size_t capacity = 100)
        : m_array(new T[capacity]), m_capacity(capacity), m_head(1), m_tail(0), m_size(0) {}
    ~ring_buffer() { delete m_array; }

    void push_back(const T& new_value) {
        increment(m_tail);
        m_size++;
        if (m_size == m_capacity) {
            // Lose some data
            increment(m_head);
            m_size--;
        }
        m_array[m_tail] = new_value;
    }

    void pop() {
        increment(m_head);
        m_size--;
    }

    T top() const { return m_array[m_head]; }

    void clear() {
        m_head = 1;
        m_tail = m_size = 0;
    }

    bool   empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }

   private:
    T*     m_array;
    size_t m_capacity;
    size_t m_head;
    size_t m_tail;
    size_t m_size;

    void increment(size_t& var) {
        var++;
        if (var == m_capacity) { var = 0; }
    }
};

}  // namespace util
}  // namespace kernel