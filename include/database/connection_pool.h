//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>
#include <boost/asio/io_context.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include "mysql/mysql_connection.h"
#include "db_connection_base.h"
#include "core/coroutine/async_mutex.h"
namespace obelisk::database {
    class connection_pool_base {
    public:
        template<typename Connection>
        std::shared_ptr<Connection> get_connection() {
            const auto conn =  this->get_connection_();
            if (!conn)
                throw std::logic_error("Get connection failed");
            auto result = std::dynamic_pointer_cast<Connection>(conn);
            if (!result)
                throw std::logic_error("Connection type not match");
            return result;
        }

        virtual ~connection_pool_base() = default;

    protected:
        virtual std::shared_ptr<db_connection_base> get_connection_() = 0;
    };

    template<typename Connection>
    class connection_pool : public connection_pool_base {
    public:
        explicit connection_pool(boost::asio::io_context&ios): ioctx_(ios) {
        }

        template<typename... Args>
        void initialize(Args... args) {
            connection_maker_ = [=]() {
                if (auto ptr = std::shared_ptr<Connection>(new Connection(), std::bind(&connection_pool::connection_reset_, this, std::placeholders::_1)); ptr->connect(args...)) {
                    ptr->reuse(true);
                    return ptr;
                }

                return std::shared_ptr<Connection>(nullptr);
            };
            std::unique_lock lock(mutex_);
            while (connections_.size() < min_) {
                connections_.push_back(connection_maker_());
            }
        }

    protected:
        std::shared_ptr<db_connection_base> get_connection_() override {
            if (!connections_.empty()) {
                std::unique_lock lock(mutex_);
                auto conn = connections_.back();
                connections_.pop_back();
                return conn;
            }

            if (auto conn = connection_maker_(); conn) return conn;
            return nullptr;
        }

        void connection_reset_(Connection* connection) {
            if (ioctx_.stopped() || !connection->reuse()) {
                delete connection;
                return;
            }
            std::unique_lock lock(mutex_);
            connections_.push_back(std::shared_ptr<Connection>(
                connection, std::bind(&connection_pool::connection_reset_, this, std::placeholders::_1)));
        }

        std::atomic_int16_t min_ = 5;
        std::mutex mutex_;
        std::atomic_int16_t max_ = 100;
        boost::asio::io_context&ioctx_;
        std::atomic_bool shutdown_ = false;
        std::vector<std::shared_ptr<Connection>> connections_;
        std::function<std::shared_ptr<Connection>()> connection_maker_;
    };

    class connection_manager : public std::enable_shared_from_this<connection_manager> {
    public:
        template<typename Connection, typename... Args>
        static void make_pool(boost::asio::io_context&ios, const std::string&key, Args... args) {
            auto mgr = self();
            if (mgr->connections_.contains(key)) return;
            auto ptr = std::make_shared<connection_pool<Connection>>(ios);
            ptr->initialize(args...);

            mgr->connections_.emplace(key, ptr);
        }

        template<typename Connection>
        static std::shared_ptr<Connection> get_connection(const std::string&key) {
            auto mgr = self();
            if (!mgr->connections_.contains(key)) return nullptr;
            return mgr->connections_[key]->get_connection<Connection>();
        }

        ~connection_manager() {
            std::cout << "Release" << std::endl;
        }

    protected:
        static std::shared_ptr<connection_manager> self() {
            if (!self_ptr_)
                self_ptr_ = std::shared_ptr<connection_manager>(new connection_manager);
            return self_ptr_;
        }

        static std::shared_ptr<connection_manager> self_ptr_;

        explicit connection_manager() = default;


        std::unordered_map<std::string, std::shared_ptr<connection_pool_base>> connections_;
    };
} // obelisk::database

#endif //CONNECTION_POOL_H
