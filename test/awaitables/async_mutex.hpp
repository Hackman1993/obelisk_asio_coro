//
// Created by admin on 2025/1/25.
//

#ifndef ASYNC_MUTEX_HPP
#define ASYNC_MUTEX_HPP

#include <boost/asio.hpp>
#include <deque>
#include <functional>
#include <type_traits>
#include <memory>
#include <queue>
class async_mutex {
public:
    explicit async_mutex(boost::asio::io_context& ioc)
        : strand_(boost::asio::make_strand(ioc)) {}

    // 禁止拷贝
    async_mutex(const async_mutex&) = delete;
    async_mutex& operator=(const async_mutex&) = delete;

    // 异步锁定，返回 awaitable
    boost::asio::awaitable<void> lock() {
        auto lock_op = std::make_shared<lock_operation>(strand_);

        // 在 strand 上执行队列操作
        co_await boost::asio::post(boost::asio::bind_executor(strand_, boost::asio::use_awaitable));

        if (!locked_ && queue_.empty()) {
            locked_ = true;
            co_return;
        }

        // 加入等待队列并等待
        queue_.push(lock_op);
        co_await lock_op->wait();
    }

    // 尝试立即获取锁
    boost::asio::awaitable<bool> try_lock() {
        co_await boost::asio::post(boost::asio::bind_executor(strand_, boost::asio::use_awaitable));
        if (!locked_ && queue_.empty()) {
            locked_ = true;
            co_return true;
        }
        co_return false;
    }

    // 解锁
    void unlock() {
        boost::asio::post(strand_, [self = this]() {
            self->locked_ = false;

            // 如果有等待者，通知下一个
            if (!self->queue_.empty()) {
                auto next = self->queue_.front();
                self->queue_.pop();
                next->complete();
            }
        });
    }

private:
    struct lock_operation {
        explicit lock_operation(boost::asio::strand<boost::asio::io_context::executor_type>& strand)
            : timer(strand) {}

        boost::asio::steady_timer timer;
        std::function<void()> continuation;

        void complete() {
            if (continuation) {
                continuation();
            }
        }

        boost::asio::awaitable<void> wait() {
            // 使用 timer 作为等待机制
            timer.expires_at(boost::asio::steady_timer::clock_type::time_point::max());
            co_await timer.async_wait(boost::asio::use_awaitable);
        }
    };

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::queue<std::shared_ptr<lock_operation>> queue_;  // 由 strand 保护
    bool locked_ = false;  // 由 strand 保护
};
#endif //ASYNC_MUTEX_HPP
