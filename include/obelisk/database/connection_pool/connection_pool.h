//
// Created by hackman on 4/23/25.
//

#ifndef CONNECTION_POOL_BASE_H
#define CONNECTION_POOL_BASE_H
#include <queue>
#include <boost/asio/awaitable.hpp>
#include <obelisk/core/coroutine/async_mutex.h>
#include <obelisk/database/core/db_connection_base.h>
#include <sahara/log/log.h>
#include <boost/mysql/error_with_diagnostics.hpp>
#include <concurrentqueue.h>

namespace obelisk::database
{
    class connection_pool_base
    {
    public:
        template <typename Connection>
        boost::asio::awaitable<std::shared_ptr<Connection>> get_connection()
        {
            auto conn = co_await this->get_connection_();
            if (!conn)
                throw std::logic_error("Get connection failed");
            auto result = std::dynamic_pointer_cast<Connection>(conn);
            if (!result)
                throw std::logic_error("Connection type not match");
            co_return result;
        }

        virtual ~connection_pool_base() = default;

    protected:
        virtual boost::asio::awaitable<std::shared_ptr<db_connection_base>> get_connection_() = 0;
    };

    template <typename Connection>
    class connection_pool : public connection_pool_base
    {
    public:
        explicit connection_pool(boost::asio::io_context& ios): ioctx_(ios), strand_(boost::asio::make_strand(ios))
        {
        }

        template <typename... Args>
        boost::asio::awaitable<void> initialize(Args... args)
        {
            connection_maker_ = [=,this](
                boost::asio::io_context& ioctx) -> boost::asio::awaitable<std::shared_ptr<Connection>>
                {
                    co_return std::shared_ptr<Connection>(new Connection(ioctx, args...),
                                                          std::bind(&connection_pool::connection_reset_, this,
                                                                    std::placeholders::_1));
                };
            while (connections_.size() < min_)
            {
                try
                {
                    auto conn = co_await connection_maker_(ioctx_);
                    if (conn)
                        connections_.enqueue(conn);
                        //connections_.push(conn);

                }
                catch (const boost::system::error_code& e)
                {
                    LOG_MODULE_CRITICAL("Database", "{} ", e.message());
                }
                catch (std::exception& e)
                {
                    LOG_MODULE_CRITICAL("Database", "{} ", e.what());
                }
            }
        }

    protected:
        boost::asio::awaitable<std::shared_ptr<db_connection_base>> get_connection_() override
        {
            co_await boost::asio::post(strand_, boost::asio::use_awaitable);
            std::shared_ptr<Connection> conn;
            if (!connections_.empty())
            {
                conn = connections_.front();
                connections_.pop();
            }
            // if (connections_.try_dequeue(conn))
            //     co_return conn;
            // else co_return co_await connection_maker_(ioctx_);
            co_await boost::asio::post(boost::asio::use_awaitable);

            if (!conn)
                conn = co_await connection_maker_(ioctx_);

            co_return conn;
        }

        void connection_reset_(Connection* connection)
        {
            if (ioctx_.stopped() || !connection->reuse())
            {
                delete connection;
                return;
            }

            try
            {
                connections_.enqueue(std::shared_ptr<Connection>(connection, std::bind(&connection_pool::connection_reset_, this, std::placeholders::_1)));
                // boost::asio::post(strand_, [connection, this]()
                // {
                //     connections_.push(std::shared_ptr<Connection>(connection, std::bind(&connection_pool::connection_reset_, this, std::placeholders::_1)));
                // });
            }
            catch (boost::mysql::error_with_diagnostics& e)
            {
                std::cout << e.what() << std::endl;
                delete connection;
            }

        }

        std::atomic_int16_t min_ = 5;
        std::atomic_int16_t max_ = 100;
        boost::asio::io_context& ioctx_;
        std::atomic_bool shutdown_ = false;
        boost::asio::strand<boost::asio::io_context::executor_type> strand_;
        moodycamel::ConcurrentQueue<std::shared_ptr<Connection>> connections_;
        //std::queue<std::shared_ptr<Connection>> connections_;
        std::function<boost::asio::awaitable<std::shared_ptr<Connection>>(boost::asio::io_context&)> connection_maker_;
    };
}

#endif //CONNECTION_POOL_BASE_H
