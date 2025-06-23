#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <deque>
#include <memory>
#ifndef OBELISK_ASYNC_MUTEX_H
#define OBELISK_ASYNC_MUTEX_H
namespace obelisk::core::coroutine
{
    class async_mutex
    {
    public:
        explicit async_mutex(boost::asio::io_context& ctx): strand_(make_strand(ctx))
        {
        }

        ~async_mutex() = default;

        // 禁止拷贝和移动
        async_mutex(const async_mutex&) = delete;
        async_mutex& operator=(const async_mutex&) = delete;
        async_mutex(async_mutex&&) = delete;
        async_mutex& operator=(async_mutex&&) = delete;

        [[nodiscard]] boost::asio::awaitable<void> lock()
        {
            // 使用 strand 确保 lock 操作序列化
            co_await dispatch(strand_, boost::asio::use_awaitable);

            if (!locked_)
            {
                locked_ = true;
                co_return;
            }

            auto promise = std::make_shared<boost::asio::steady_timer>(
                strand_, // 定时器绑定到 strand
                boost::asio::steady_timer::time_point::max()
            );

            waiters_.push_back(promise);
            try{
                auto [ec] = co_await promise->async_wait(boost::asio::as_tuple(boost::asio::use_awaitable));
            }catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }

        void unlock()
        {
            // 使用 strand 包装 unlock 操作
            post(strand_, [this]
            {
                if (!locked_)
                {
                    return;
                }

                if (waiters_.empty())
                {
                    locked_ = false;
                    return;
                }

                auto promise = waiters_.front();
                waiters_.pop_front();
                promise->cancel();
            });
        }

    private:
        boost::asio::strand<boost::asio::io_context::executor_type> strand_;
        bool locked_ = false;
        std::deque<std::shared_ptr<boost::asio::steady_timer>> waiters_;
    };
}
#endif