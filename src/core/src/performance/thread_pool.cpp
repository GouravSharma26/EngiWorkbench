#include "workbench/performance/thread_pool.hpp"
#include <algorithm>

namespace workbench {
namespace performance {

ThreadPool::ThreadPool(size_t thread_count) {
    if (thread_count == 0) {
        thread_count = 1;
    }
    workers_.reserve(thread_count);

    for (size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    this->condition_.wait(lock, [this]() {
                        return this->stop_.load(std::memory_order_acquire) || !this->tasks_.empty();
                    });

                    if (this->stop_.load(std::memory_order_acquire) && this->tasks_.empty()) {
                        return;
                    }

                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }

                try {
                    task();
                } catch (...) {
                    // Suppress uncaught task exceptions from crashing worker thread
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::stop() {
    bool expected = false;
    if (stop_.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        condition_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
}

void ThreadPool::parallel_for(
    size_t begin, 
    size_t end, 
    const std::function<void(size_t index)>& body,
    const std::atomic<bool>* cancel_token
) {
    if (begin >= end) {
        return;
    }

    std::atomic<size_t> current_idx{begin};
    std::atomic<bool> error_occurred{false};
    std::exception_ptr caught_ex = nullptr;
    std::mutex ex_mutex;

    size_t thread_count = workers_.size();
    std::vector<std::future<void>> futures;
    futures.reserve(thread_count);

    for (size_t t = 0; t < thread_count; ++t) {
        futures.push_back(enqueue([&, cancel_token]() {
            while (true) {
                if (cancel_token && cancel_token->load(std::memory_order_relaxed)) {
                    break;
                }
                if (error_occurred.load(std::memory_order_relaxed)) {
                    break;
                }

                size_t i = current_idx.fetch_add(1, std::memory_order_relaxed);
                if (i >= end) {
                    break;
                }

                try {
                    body(i);
                } catch (...) {
                    std::lock_guard<std::mutex> lock(ex_mutex);
                    if (!error_occurred.exchange(true)) {
                        caught_ex = std::current_exception();
                    }
                    break;
                }
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    if (caught_ex) {
        std::rethrow_exception(caught_ex);
    }
}

} // namespace performance
} // namespace workbench

