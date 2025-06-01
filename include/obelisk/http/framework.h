//
// Created by hackman on 4/25/25.
//

#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include <obelisk/database/db_pool.h>
#include <obelisk/database/mysql/mysql_connection.h>
#include <obelisk/http/framework/config.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "http_server.h"

namespace obelisk::http
{
    class framework
    {
    public:
        framework(const framework&) = delete;
        framework& operator=(const framework&) = delete;

        static framework& init()
        {
            std::call_once(flag_, &framework::set_);
            return *instance_;
        };

        static void module(const std::initializer_list<std::unique_ptr<module::base_module>>& modules)
        {
            self().server_.module(modules);
        }

        static void middleware(const std::initializer_list<std::shared_ptr<middleware::base_middleware>>& middlewares)
        {
            self().server_.reg_middlewares(middlewares);
        }

        static void run()
        {
            auto listens = config::get<nlohmann::json::array_t>("network.addresses", nlohmann::json::array());
            for (const auto &listen: listens)
            {
                auto str = listen.get<std::string>();
                boost::regex url_regex(R"(^(?<address>[a-zA-Z0-9.-]+):(?<port>\d+)$)", boost::regex::icase | boost::regex::no_char_classes);

                boost::smatch match;
                if (!boost::regex_match(str, match, url_regex))
                    THROW(sahara::exception::exception_base, "Invalid Address!", "Obelisk Framework");
                self().server_.listen(match["address"].str(),boost::lexical_cast<std::uint16_t>(match["port"].str()));
            }
            self().io_context_.run();
        }
    private:
        static framework& self()
        {
            if (!instance_)
                throw std::logic_error("Framework not initialized");
            return *instance_;
        }
        static void set_()
        {
            instance_ = new framework();
        }

        explicit framework():  server_(io_context_)
        {
            for (auto& item : config::get<nlohmann::json::object_t>("database", nlohmann::json::object_t{}))
            {
                const auto database = config::get<std::string>(std::format("database.{}.database",item.first), "default");
                const auto host = config::get<std::string>(std::format("database.{}.host",item.first), "127.0.0.1");
                const auto username = config::get<std::string>(std::format("database.{}.user",item.first), "root");
                const auto password = config::get<std::string>(std::format("database.{}.password",item.first), "password");
                const auto port = config::get<std::uint16_t>(std::format("database.{}.port",item.first), 3306);
                database::db_pool::make_pool<mysql_connection>(io_context_, item.first, host, port, username, password, database);
            }
        }
        ~framework()
        {
            if (instance_)
                delete instance_;
        };

        static framework* instance_; // 静态实例指针
        static std::once_flag flag_; // 确保初始化只发生一次
        boost::asio::io_context io_context_;
        http_server server_;
    };
}
#endif //FRAMEWORK_H
