#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace asciinema {

template <typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(size_t capacity) : capacity_(capacity) {}

    void push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push(std::move(item));
        lock.unlock();
        not_empty_.notify_one();
    }

    bool try_push(T item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() >= capacity_) return false;
        queue_.push(std::move(item));
        not_empty_.notify_one();
        return true;
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { return !queue_.empty() || stopped_; });
        if (stopped_ && queue_.empty()) return T{};
        T item = std::move(queue_.front());
        queue_.pop();
        lock.unlock();
        not_full_.notify_one();
        return item;
    }

    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        T item = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return item;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t capacity() const { return capacity_; }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    size_t capacity_;
    bool stopped_ = false;
};

}  
