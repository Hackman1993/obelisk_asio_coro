//
// Created by hackman on 4/25/25.
//

#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include <boost/asio/awaitable.hpp>
#include <memory>
#include <obelisk/database/db_pool.h>
#include <obelisk/database/mysql/mysql_connection.h>
#include <obelisk/http/framework/config.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "http_server.h"
#include "obelisk/filesystem/filesystem.h"
#include "obelisk/http/module/base_module.h"

namespace obelisk::http
{
    class framework
    {
    public:
        framework(const framework&) = delete;
        framework& operator=(const framework&) = delete;
        explicit framework(boost::asio::io_context& ctx): io_context_(ctx), server_(ctx){}

        void init(){
            boost::asio::co_spawn(io_context_, [this]() -> boost::asio::awaitable<void> {
                co_await init_();
                co_return;
            }, boost::asio::detached);
        }

        void start()
        {
            server_.start();
        }

        void module(const std::initializer_list<std::shared_ptr<module::base_module>>& modules)
        {
            for(auto & module:modules){
                registered_modules_.push_back(module);
            }
        }

        void middleware(const std::initializer_list<std::shared_ptr<middleware::base_middleware>>& middlewares)
        {
            server_.reg_middlewares(middlewares);
        }

        static void register_fs(const std::string& key, const std::function<std::shared_ptr<fs::detail::base_filesystem>(const nlohmann::json& config)>& maker)
        {
            filesystem::register_fs(key, maker);
        }

        ~framework() = default;
    private:
        boost::asio::awaitable<void> init_()
        {
            for (auto& item : config::get<nlohmann::json::object_t>("database", nlohmann::json::object_t{}))
            {
                const auto database = config::get<std::string>(std::format("database.{}.database", item.first), "default");
                const auto host = config::get<std::string>(std::format("database.{}.host", item.first), "127.0.0.1");
                const auto username = config::get<std::string>(std::format("database.{}.user", item.first), "root");
                const auto password = config::get<std::string>(std::format("database.{}.password", item.first), "password");
                const auto port = config::get<std::uint16_t>(std::format("database.{}.port", item.first), 3306);
                co_await database::db_pool::make_pool<mysql_connection>(io_context_, item.first, host, port, username, password, database);
            }
            auto listens = config::get<nlohmann::json::array_t>("network.addresses", nlohmann::json::array());
            for (const auto& listen : listens)
            {
                auto str = listen.get<std::string>();
                boost::regex url_regex(R"(^(?<address>[a-zA-Z0-9.-]+):(?<port>\d+)$)",boost::regex::icase | boost::regex::no_char_classes);
                boost::smatch match;
                if (!boost::regex_match(str, match, url_regex))
                    THROW(sahara::exception::exception_base, "Invalid Address!", "Obelisk Framework");
                server_.listen(match["address"].str(), boost::lexical_cast<std::uint16_t>(match["port"].str()));
            }
            filesystem::initialize();
        }
        boost::asio::io_context& io_context_;
        http_server server_;
        std::vector<std::shared_ptr<obelisk::http::module::base_module>> registered_modules_;
    };
}
#endif //FRAMEWORK_H
