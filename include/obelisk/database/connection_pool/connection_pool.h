//
// Created by hackman on 4/23/25.
//

#ifndef CONNECTION_POOL_BASE_H
#define CONNECTION_POOL_BASE_H
#include <obelisk/core/coroutine/async_mutex.h>
#include <obelisk/database/core/db_connection_base.h>
#include <sahara/log/log.h>

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
        explicit connection_pool(boost::asio::io_context& ios): ioctx_(ios), mutex_(ios)
        {
        }

        template <typename... Args>
        void initialize(Args... args)
        {
            connection_maker_ = [=,this](boost::asio::io_context& ioctx)
            {
                return std::shared_ptr<Connection>(new Connection(ioctx, args...),
                                                   std::bind(&connection_pool::connection_reset_, this,
                                                             std::placeholders::_1));
            };
            std::unique_lock lock(mutex_);
            while (connections_.size() < min_)
            {
                try
                {
                    auto conn = connection_maker_(ioctx_);
                    if (conn)
                        connections_.push_back(conn);
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
            co_await mutex_.lock();
            std::shared_ptr<Connection> conn;
            if (!connections_.empty())
            {
                conn = connections_.back();
                connections_.pop_back();
            }
            mutex_.unlock();
            if (conn)
                co_return conn;

            if (conn = connection_maker_(ioctx_); conn) co_return conn;
            co_return nullptr;
        }

        void connection_reset_(Connection* connection)
        {
            if (ioctx_.stopped() || !connection->reuse())
            {
                delete connection;
                return;
            }
            std::unique_lock lock(mutex_);
            connections_.push_back(std::shared_ptr<Connection>(
                connection, std::bind(&connection_pool::connection_reset_, this, std::placeholders::_1)));
        }

        std::atomic_int16_t min_ = 5;
        obelisk::core::coroutine::async_mutex mutex_;
        std::atomic_int16_t max_ = 100;
        boost::asio::io_context& ioctx_;
        std::atomic_bool shutdown_ = false;
        std::vector<std::shared_ptr<Connection>> connections_;
        std::function<std::shared_ptr<Connection>(boost::asio::io_context&)> connection_maker_;
    };
}

#endif //CONNECTION_POOL_BASE_H
