//
// Created by Hackman.Lo on 2023/12/28.
//

#ifndef ASYNC_MUTEX2_H
#define ASYNC_MUTEX2_H
#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/io_service.hpp>

namespace obelisk::core::coroutine {
    class async_mutex2;

    class async_mutex_scoped_lock {
    public:
        async_mutex_scoped_lock(int val): val_(val) {
        };

    protected:
        int val_;
    };

    class async_mutex2 {
    public:
        ~async_mutex2() = default;

        template<typename CompletionToken>
        auto async_lock(CompletionToken&&token) {
            return boost::asio::async_initiate<CompletionToken, int (int)>([](auto handler, async_mutex2* self) {
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }, token, this);
        }

    private:
        std::mutex mutex_;
        std::unique_ptr<std::thread> thread_;
    };

    template<typename CompletionToken>
    auto async_lock(CompletionToken&&token, std::mutex&mutex) {
        return boost::asio::async_initiate<CompletionToken, void (async_mutex_scoped_lock)>([&](auto handler) {
            std::thread thread([&]() {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                handler(async_mutex_scoped_lock(2));
            });
            thread.detach();
        }, token);
    }
} // obelisk::core::coroutine

#endif //ASYNC_MUTEX_H
