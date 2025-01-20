//
// Created by Hackman.Lo on 2024/1/3.
//

#ifndef ASYNC_SCOPED_LOCK_H
#define ASYNC_SCOPED_LOCK_H
#include <boost/asio.hpp>
#include <boost/asio/experimental/basic_channel.hpp>
#include <boost/asio/experimental/channel.hpp>

namespace obelisk::core::coroutine {
//    struct Awaiter : public boost::cobalt::enable_awaitables<bool> {
//        Awaiter(std::timed_mutex& mutex, std::atomic_bool& operated) : mutex_(mutex), operated_(operated) {
//        }
//
//        bool await_ready() const noexcept { return false; }
//
//        void await_suspend(std::coroutine_handle<> coro) {
//            std::thread([&,coro]() {
//                std::this_thread::sleep_for(std::chrono::seconds(3));
//                operated_ = true;
//                coro.resume();
//            }).detach();
//        }
//
//        bool await_resume() const noexcept { return true; }
//        std::timed_mutex& mutex_;
//        std::atomic_bool& operated_;
//    };
//
//    class async_scoped_lock {
//    public:
//        explicit async_scoped_lock(std::timed_mutex&mutex): mutex_(mutex) {
//        };
//
//        auto async_lock() {
//            return Awaiter(mutex_, locked_);
//        }
//
//        ~async_scoped_lock() {
//            if (locked_)
//                mutex_.unlock();
//        }
//
//    protected:
//        std::timed_mutex&mutex_;
//        std::atomic_bool locked_;
//    };
} // obelisk::core::coroutine

#endif //ASYNC_SCOPED_LOCK_H
