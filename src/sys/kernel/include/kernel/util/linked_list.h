#pragma once

#include <stddef.h>

namespace kernel {
namespace util {

template <typename T>
class linked_list {
   public:
    struct Node {
        T*    val;
        Node* m_next;
        Node(T* v) : val(v), m_next(nullptr) {}
    };

    void push_back(T* value) {
        auto* val = new Node(value);

        if (m_head == nullptr && m_tail == nullptr) {
            m_head = val;
            m_tail = val;
        } else {
            m_tail->m_next = val;
            m_tail         = val;
        }
        m_size++;
    }

    T* pop_front() {
        if (m_head == nullptr) { return nullptr; }
        if (m_head->m_next == nullptr) { m_tail = nullptr; }

        auto* to_return = m_head;
        T*    retval    = to_return->val;
        m_head          = m_head->m_next;
        m_size--;
        delete to_return;
        return retval;
    }

    void remove(T* needle) {
        Node* prev  = nullptr;
        Node* found = nullptr;
        for (auto* cur = m_head; cur != nullptr; cur = cur->m_next) {
            if (needle == cur->val) {
                found = cur;
                return;
            }
            prev = cur;
        }

        if (found == nullptr) {
            return;
        } else if (prev == nullptr) {
            pop_front();
        } else {
            prev->m_next = found->m_next;  // Element is removed.
        }
        m_size--;
    }

    Node* front() const {
        if (m_head == nullptr) { return nullptr; }
        return m_head;
    }

    bool   empty() const { return m_head == nullptr; }
    size_t size() const { return m_size; };

    struct iterator {
        iterator(Node* ptr) : m_ptr(ptr) {}
        T& operator*() const { return *m_ptr->val; }
        T* operator->() { return m_ptr->val; }

        iterator& operator++() {
            m_ptr = m_ptr->m_next;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; };

       private:
        Node* m_ptr;
    };

    iterator begin() { return iterator(m_head); }
    iterator end() { return iterator(nullptr); }

   private:
    Node*  m_head = nullptr;
    Node*  m_tail = nullptr;
    size_t m_size = 0;
};

}  // namespace util
}  // namespace kernel