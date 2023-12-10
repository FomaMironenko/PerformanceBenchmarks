#include <thread>
#include <atomic>
#include <mutex>
#include <deque>
#include <cassert>
#include <optional>

class Spinlock {
public:
    void lock() {
        while (locked.test_and_set(std::memory_order_acq_rel)) {
            std::this_thread::yield();
        }
    }

    void unlock() {
        locked.clear(std::memory_order_release);
    }

private:
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
};


template <typename T>
class SPSCQueue
{
public:
    SPSCQueue() {}

    bool push(T val) {
        std::lock_guard guard(for_produce_mtx_);
        for_produce_.push_back(std::move(val));
        return true;
    }

    std::optional<T> pop() {
        if (for_consume_.empty()) {
            std::lock_guard guard(for_produce_mtx_);
            // Constant work here
            if (!for_produce_.empty()) {
                std::swap(for_produce_, for_consume_);
            }
        }
        if (for_consume_.empty()) {
            return std::nullopt;
        }
        T val = std::move(for_consume_.front());
        for_consume_.pop_front();
        return val;
    }

private:
    // Used only by consumer
    std::deque<T> for_consume_;
    // Used by both producer and consumer : consumer steales values from here
    std::deque<T> for_produce_;
    Spinlock for_produce_mtx_;
};
