//
// Created by Hackman.Lo on 2024/1/3.
//

#ifndef ASYNC_SCOPED_LOCK_H
#define ASYNC_SCOPED_LOCK_H
#include <boost/asio.hpp>
namespace obelisk::core::coroutine {

    template<typename Self>
    struct operation_worker {
        operation_worker(Self& self, std::timed_mutex& mutex, std::atomic_bool& operated):
            self_(self), mutex_(mutex), operated_(operated){}
        void operator()() {
            if(mutex_.try_lock_for(std::chrono::seconds(5))) {
                operated_ = true;
                self_.complete();
            }
            self_.complete();
        }
        Self& self_;
        std::timed_mutex& mutex_;
        std::atomic_bool& operated_;
    };

    struct lock_operation {
        lock_operation(std::timed_mutex& mutex, std::atomic_bool& operated):mutex_(mutex), operated_(operated){};
        template <typename Self>
        void operator()(Self& self) {
            //impl(self, mutex_, operated_);
            // if(mutex_.try_lock_for(std::chrono::seconds(5))) {
            //     operated_ = true;
            //     self.complete();
            // }

            self.complete();
            // std::async(operation_worker{self, mutex_, operated_});
        }
    protected:
        template <typename T>
        void impl(T& self, std::timed_mutex& mutex, std::atomic_bool& operated)  {

        }
        std::timed_mutex& mutex_;
        std::atomic_bool& operated_;
    };

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
            return boost::asio::async_compose<CompletionToken, void()>(lock_operation{this->mutex_, this->operated_}, token);
        }

        ~async_scoped_lock() {
            if(operated_)
                mutex_.unlock();
        }
    protected:
        std::timed_mutex& mutex_;
        std::atomic_bool operated_;
    };
} // obelisk::core::coroutine

#endif //ASYNC_SCOPED_LOCK_H
