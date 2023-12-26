//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H
#include <memory>
#include <string>
#include <unordered_map>
#include <boost/asio/io_context.hpp>

#include "db_connection_base.h"

namespace obelisk::database {
    class connection_pool_base {
    public:
        template <typename Connection>
        std::shared_ptr<Connection> get_connection() {
            const auto conn = this->get_connection_();
            if(!conn)
                throw std::logic_error("Get connection failed");
            auto result = std::dynamic_pointer_cast<Connection>(conn);
            if(!result)
                throw std::logic_error("Connection type not match");
            return result;
        }

        virtual ~connection_pool_base() = default;
    protected:
        virtual std::shared_ptr<db_connection_base> get_connection_() = 0;
    };

    template<typename Connection, typename... Args>
    class connection_pool: public connection_pool_base{
    public:
        explicit connection_pool(boost::asio::io_context& ios): ioctx_(ios){}
        ~connection_pool() override = default;

        void initialize(Args... args) {
            auto lambda = [&]() {
                while (true) {
                    std::unique_lock lock(mutex_);
                    while(connections_.size() < max_) {
                        connections_.push_back(make_connection_(args...));
                    }
                }
            };
        }


    protected:
        std::shared_ptr<db_connection_base> get_connection_() override {
            return nullptr;
        }
        void connection_reset_(Connection *connection) {
            if(shutdown_) {
                delete connection;
                return;
            }
            std::unique_lock lock(mutex_);
            connections_.push_back(std::shared_ptr<Connection>(connection, std::bind(&connection_pool<Connection>::connection_reset_, this, std::placeholders::_1)));
        }
        std::shared_ptr<Connection> make_connection_(Args... args) {
            return std::shared_ptr<Connection>(new Connection(args...), std::bind(&connection_pool<Connection>::connection_reset_, this, std::placeholders::_1));
        };

        std::mutex mutex_;
        std::atomic_int16_t min_ = 5;
        std::atomic_int16_t max_ = 100;
        boost::asio::io_context& ioctx_;
        std::atomic_bool shutdown_ = false;
        std::unique_ptr<std::thread> connection_generation_;
        std::vector<std::shared_ptr<Connection>> connections_;
    };

    class connection_manager {
    public:
        explicit connection_manager(boost::asio::io_context& ios): ioctx_(ios){}

        template <typename Connection, typename... Args>
        void initialize(const std::string& key, Args... args) {
            if(connections_.contains(key)) return;
            auto pool = std::make_shared<connection_pool<Connection>>(ioctx_);
            pool->initialize(args...);
            connections_[key] = pool;
        }

        template <typename Connection>
        std::shared_ptr<Connection> get_connection(const std::string& key) {
            if(! connections_.contains(key)) return nullptr;
            return connections_[key]->get_connection<Connection>();
        }
    protected:
        boost::asio::io_context& ioctx_;
        std::unordered_map<std::string, std::shared_ptr<connection_pool_base>> connections_;
    };

} // obelisk::database

#endif //CONNECTION_POOL_H
