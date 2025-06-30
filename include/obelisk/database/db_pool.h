//
// Created by hackman on 4/23/25.
//

#ifndef DB_POOL_H
#define DB_POOL_H

#include <boost/asio/awaitable.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <sahara/exception/exception_base.h>
#include "obelisk/core/coroutine/async_scoped_lock.h"
#include "connection_pool/connection_pool.h"

namespace obelisk::database
{
    class db_pool : public std::enable_shared_from_this<db_pool>
    {
    public:
        template <typename Connection, typename... Args>
        static boost::asio::awaitable<void> make_pool(boost::asio::io_context& ios, const std::string& key, Args... args)
        {
            if (self().connections_.contains(key)) co_return;
            auto ptr = std::make_shared<connection_pool<Connection>>(ios);
            co_await ptr->initialize(args...);

            self().connections_.emplace(key, ptr);
            co_return;
        }

        template <typename Connection>
        static boost::asio::awaitable<std::shared_ptr<Connection>> get_connection(const std::string& key)
        {
            if (!self().connections_.contains(key))
                THROW(sahara::exception::exception_base, "Connection pool not found", "Obelisk");
            co_return co_await self().connections_[key]->get_connection<Connection>();
        }
        db_pool(const db_pool&) = delete;
        db_pool& operator=(const db_pool&) = delete;
    protected:
        db_pool() = default;
        ~db_pool()= default;
        static db_pool& self()
        {
            if (instance_ == nullptr)
                instance_ = new db_pool();
            return *instance_;
        }
        inline static db_pool* instance_ = nullptr;
        std::unordered_map<std::string, std::shared_ptr<connection_pool_base>> connections_;
    };
} // obelisk::database
#endif //DB_POOL_H
