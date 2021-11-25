#pragma once

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/thread.h>

namespace kernel {
class scheduler {
   public:
    class scheduler_queue {
        thread* head;
        thread* tail;

       public:
        thread_state state;

        void enqueue(thread* t) {
            t->state = state;
            t->next  = nullptr;

            if (head == nullptr && tail == nullptr) {
                head = t;
                tail = t;
            } else {
                tail->next = t;
                tail       = t;
            }
        }

        thread* dequeue() {
            if (head == nullptr) { panic("Attempted to dequeue a thread from an empty queue"); }
            thread* to_return = head;
            head              = head->next;

            if (head == nullptr) { tail = nullptr; }

            return to_return;
        }

        thread* top() { return head; }
        bool    empty() { return head == nullptr; }

        size_t size() {
            size_t n = 0;
            for (auto t = head; t != nullptr; t = t->next) { n++; }
            return n;
        }
    };

   public:
    static task* kernel_task;

    static void schedule();
    static void init(phys_addr_t kernel_vas);
    static void enqueue_new(thread* t);
    static void enqueue(thread* t);
    static void terminate(thread* t);
    static void debug();

    static void lock();
    static void unlock();

   private:
    static scheduler_queue ready_queue;
    static uint64_t        last_schedule_ns;
    static int             scheduler_lock;
};
}  // namespace kernel