#pragma once

#include <kernel/panic.h>
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
                t->next = t;
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
    };

    static scheduler_queue ready_queue;
    static void            schedule();
    static void            init();
    static void            enqueue_new(thread* t);
};
}  // namespace kernel