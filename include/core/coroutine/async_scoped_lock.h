//
// Created by Hackman.Lo on 2024/1/3.
//

#ifndef ASYNC_SCOPED_LOCK_H
#define ASYNC_SCOPED_LOCK_H
#include <boost/asio.hpp>
namespace obelisk::core::coroutine {

    class async_scoped_lock {
    public:
        explicit async_scoped_lock(std::timed_mutex& mutex): mutex_(mutex){};
        template <typename CompletionToken, class Rep, class Period>
        boost::asio::awaitable<bool> async_lock_for(const std::chrono::duration<Rep, Period>& timeout, CompletionToken&& token) {
            return boost::asio::async_compose<CompletionToken, void(bool)>([&lock = this->mutex_, &timeout, &operated = this->operated_](auto&& self) {
               std::thread([self = std::move(self), &lock, duration = timeout, &operated]() mutable {
                   if(!lock.try_lock_for(duration))
                       self.complete(false);
                   operated = true;
                   self.complete(true);
               }).detach();
           }, token);
        }

        template <typename CompletionToken>
        auto async_lock(CompletionToken&& token) {
            return boost::asio::async_compose<CompletionToken, void()>([&lock = this->mutex_, &operated = this->operated_](auto&& self) {
               std::thread([self = std::move(self), &lock, &operated]() mutable {
                   lock.lock();
                   operated = true;
                   self.complete();
               }).detach();
           }, token);
        }

        ~async_scoped_lock() {
            if(operated_)
                mutex_.unlock();
        }
    protected:
        static boost::asio::awaitable<void> async_lock_(auto self, std::atomic_bool& operated, std::timed_mutex& mutex) {

        }
        std::atomic_bool operated_ = false;
        std::timed_mutex& mutex_;
    };
} // obelisk::core::coroutine

#endif //ASYNC_SCOPED_LOCK_H
