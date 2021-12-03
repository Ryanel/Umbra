#pragma once

#include <stddef.h>

namespace kernel {
namespace util {

template <typename Node>
class linked_list_inline {
   public:
    void push_back(Node* val) {
        val->m_next = nullptr;

        if (m_head == nullptr && m_tail == nullptr) {
            m_head = val;
            m_tail = val;
        } else {
            m_tail->m_next = val;
            m_tail         = val;
        }
        m_size++;
    }

    void push_front(Node* val) {}

    Node* pop_front() {
        if (m_head == nullptr) { return nullptr; }

        Node* to_return = m_head;
        m_head          = m_head->m_next;
        if (m_head == nullptr) { m_tail = nullptr; }
        m_size--;
        return to_return;
    }

    void remove(Node* needle) {
        Node* prev  = nullptr;
        Node* found = nullptr;
        for (Node* cur = m_head; cur != nullptr; cur = cur->m_next) {
            if (needle == cur) {
                found = cur;
                break;
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

    Node*  front() const { return m_head; }
    bool   empty() const { return m_head == nullptr; }
    size_t size() const { return m_size; };

   private:
    Node*  m_head = nullptr;
    Node*  m_tail = nullptr;
    size_t m_size = 0;
};
}  // namespace util
}  // namespace kernel