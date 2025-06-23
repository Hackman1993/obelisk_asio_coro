#include <string>
#include <boost/asio/awaitable.hpp>
#include <boost/mysql/results.hpp>
#include <obelisk/database/migration/migration.h>
#include <obelisk/database/migration/table_blueprint.h>
#include <obelisk/database/mysql/mysql_connection.h>
//
// Created by hackman on 4/27/25.
//
namespace obelisk::database::migration
{

    boost::asio::awaitable<void> migration::create(const std::string& table, const std::function<void (table_blueprint&)>& call)
    {
        table_blueprint bp = co_await table_bp(table, true, call);
        co_return;
    }

    boost::asio::awaitable<void> migration::table(const std::string& table, const std::function<void (table_blueprint&)>& call)
    {
        table_blueprint blueprint = co_await table_bp(table, false, call);
        co_return;
    }

    boost::asio::awaitable<void> migration::drop_if_exists(const std::string&& table)
    {
        co_return;
    }

    boost::asio::awaitable<table_blueprint> migration::table_bp(const std::string& table, const bool create, const std::function<void (table_blueprint&)>& call, const std::string& inst)
    {
        table_blueprint blueprint(table, create, self().prefix_);
        call(blueprint);
        const auto commands = blueprint.describe();

        const auto connection  = co_await db_pool::get_connection<mysql_connection>(inst);
        for (auto &cmd: commands)
        {
            try{
                //std::cout << cmd.sql() << std::endl;
                co_await connection->co_query(cmd.sql());
            }catch (std::exception&)
            {
                if (cmd.required())
                    throw;
            }
        }
        co_return blueprint;
    }

    migration& migration::self() {
        static migration instance;
        return instance;
    }

    migration::migration()
    {
        prefix_ = http::config::get<std::string>("database.default.prefix", "t_");
    }
}