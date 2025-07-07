//
// Created by Hackman.Lo on 2024/7/11.
//

#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/mysql.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <obelisk/database/core/db_connection_base.h>
#include <sahara/log/log.h>

namespace obelisk::database::builder::detail
{
    class base_statement;
}

class mysql_connection : public obelisk::database::db_connection_base, public boost::mysql::any_connection{
public:

    mysql_connection(boost::asio::io_context& ioctx, const std::string& server, const std::uint16_t port, const std::string& username,const std::string& password,const std::string& database):
        db_connection_base(ioctx), boost::mysql::any_connection(ioctx), keep_alive_timer_(ioctx) {

        boost::asio::ip::tcp::resolver resolver(ioctx);
        auto endpoints = resolver.resolve(server, std::to_string(port));
        boost::system::error_code ec;
        boost::mysql::diagnostics diagnostics;
        boost::mysql::connect_params connect_params;
        connect_params.server_address.emplace_host_and_port(server, port);
        connect_params.database = database;
        connect_params.username = username;
        connect_params.password = password;
        connect(connect_params, ec, diagnostics);
        boost::mysql::throw_on_error(ec, diagnostics);
        set_character_set(boost::mysql::character_set("utf8mb4"), ec, diagnostics);
        boost::mysql::throw_on_error(ec, diagnostics);
        boost::asio::co_spawn(ioctx, [this]() -> boost::asio::awaitable<void> {
            while (true) {
                try
                {
                    keep_alive_timer_.expires_after(this->refresh_rate());
                    co_await keep_alive_timer_.async_wait(boost::asio::use_awaitable);
                    co_await co_query("SELECT 1;");
                    LOG_MODULE_CRITICAL("Database", "Connection Refresh{}", "");
                }catch (boost::system::error_code& e)
                {
                    if (e == boost::asio::error::operation_aborted)
                        break;
                }

                keep_alive_timer_.expires_after(this->refresh_rate());
            }
            co_return;
        }, boost::asio::detached);
    }

    std::chrono::duration<std::uint32_t> refresh_rate() override
    {
        return std::chrono::seconds(30);
    }

    template <typename ResultType = boost::mysql::results, typename=std::enable_if_t<
        !std::is_same_v<ResultType, void>
    >>
    boost::asio::awaitable<ResultType> co_query(const std::string& query)
    {
        std::string sql = query.ends_with(";")? query:query+";";
        LOG_MODULE_INFO("Database", "Running SQL: {}", sql);
        //std::cout << sql << std::endl;
        ResultType results;
        boost::mysql::diagnostics diagnostics;
        if (auto [ec] = co_await any_connection::async_execute(sql, results, diagnostics, boost::asio::as_tuple(boost::asio::use_awaitable)); ec)
        {
            LOG_MODULE_CRITICAL("Database", "MySQL Query Error: {}", ec.what());
            auto message = !diagnostics.client_message().empty()? diagnostics.client_message():diagnostics.server_message();
            throw std::logic_error(ec.what());
        }
        co_return results;
    }

    template <typename ResultType>
    requires std::is_same_v<ResultType, void>
    boost::asio::awaitable<void> co_query(const std::string& query)
    {

        std::string sql = query.ends_with(";")? query:query+";";
        //std::cout << sql << std::endl;
        LOG_MODULE_INFO("Database", "Running SQL: {}", sql);
        boost::mysql::results results;
        boost::mysql::diagnostics diagnostics;
        if (auto [ec] = co_await async_execute(sql, results, diagnostics, boost::asio::as_tuple(boost::asio::use_awaitable)); ec)
        {
            LOG_MODULE_CRITICAL("Database", "MySQL Query Error: {}", ec.what());
            const auto message = !diagnostics.client_message().empty()? diagnostics.client_message():diagnostics.server_message();
            throw std::logic_error(ec.what());
        }
        co_return;
    }

    template <typename Rt, typename T>
    requires std::is_base_of_v<obelisk::database::builder::detail::base_statement, T>
    boost::asio::awaitable<Rt> co_query(T& builder)
    {
        co_return co_await co_query<Rt>(&builder->compile());
    }

    void refresh() override;

    bool reset() override;

    ~mysql_connection() override;

private:
    boost::asio::steady_timer keep_alive_timer_;
};



#endif //MYSQL_CONNECTION_H
