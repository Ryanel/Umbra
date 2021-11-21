#pragma once

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/thread.h>

namespace kernel {
class scheduler {
   public:
    class scheduler_queue {
        thread*      head;
        thread*      tail;
        thread_state state;

       public:
        void enqueue(thread* t) {
            t->state = state;
            if (head == nullptr && tail == nullptr) {
                t->next = nullptr;
                head    = t;
                tail    = t;
            } else {
                tail->next = t;
                tail       = t;
            }
        }

        thread* dequeue() {
            if (head == nullptr) { panic("Attempted to dequeue a thread from an empty queue"); }
            thread* to_return = head;
            head              = head->next;
            return to_return;
        }

        bool empty() { return head == nullptr; }

        size_t size() {
            size_t n = 0;
            for (auto t = head; t != nullptr; t = t->next) { n++; }
            return n;
        }
    };

   public:
    static void schedule();
    static void init(phys_addr_t kernel_vas);
    static void enqueue_new(thread* t);
    static void enqueue(thread* t);
    static void debug();

   private:
    static scheduler_queue ready_queue;
};
}  // namespace kernel