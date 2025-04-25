//
// Created by hackman on 4/25/25.
//

#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include <obelisk/database/db_pool.h>
#include <obelisk/database/mysql/mysql_connection.h>
#include <obelisk/http/framework/config.h>

#include "http_server.h"

namespace obelisk::http
{
    class framework
    {
    public:
        framework(const framework&) = delete;
        framework& operator=(const framework&) = delete;

        static framework& init(boost::asio::io_context& io_context)
        {
            std::call_once(flag_, &framework::set_, io_context);
            return *instance_;
        };
    private:
        static framework& self()
        {
            if (!instance_)
                throw std::logic_error("Framework not initialized");
            return *instance_;
        }
        static void set_(boost::asio::io_context& io_context)
        {
            instance_ = new framework(io_context);
        }

        explicit framework(boost::asio::io_context& io_context): io_context_(io_context), server_(io_context)
        {
            const auto database = config::get<std::string>("database.default.database", "default");
            const auto host = config::get<std::string>("database.default.host", "127.0.0.1");
            const auto username = config::get<std::string>("database.default.username", "root");
            const auto password = config::get<std::string>("database.default.password", "password");
            const auto port = config::get<std::uint16_t>("database.default.port", 3306);
            database::db_pool::make_pool<mysql_connection>(io_context, "default", host, port, username, password, database);
        }
        ~framework()
        {
            if (instance_)
                delete instance_;
        };

        static framework* instance_; // 静态实例指针
        static std::once_flag flag_; // 确保初始化只发生一次
        boost::asio::io_context& io_context_;
        http_server server_;
    };
}
#endif //FRAMEWORK_H
