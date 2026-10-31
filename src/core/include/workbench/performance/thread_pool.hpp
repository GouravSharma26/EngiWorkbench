#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <stdexcept>
#include <memory>

namespace workbench {
namespace performance {

/// A bounded C++17 worker thread pool for parallel execution.
class ThreadPool {
public:
    /// Constructs a pool with a bounded number of worker threads.
    explicit ThreadPool(size_t thread_count);

    /// Destructor joins all worker threads cleanly.
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /// Enqueues a task returning a std::future for the result.
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (stop_.load(std::memory_order_acquire)) {
                throw std::runtime_error("Cannot enqueue task on stopped ThreadPool.");
            }
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return res;
    }

    /// Parallel loop over a range [begin, end) with dynamic/atomic index distribution.
    /// Supports a cancellation flag.
    void parallel_for(
        size_t begin, 
        size_t end, 
        const std::function<void(size_t index)>& body,
        const std::atomic<bool>* cancel_token = nullptr
    );

    /// Number of active worker threads.
    size_t worker_count() const noexcept {
        return workers_.size();
    }

    /// Stops accepting new tasks and wakes up workers.
    void stop();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_{false};
};

} // namespace performance
} // namespace workbench

