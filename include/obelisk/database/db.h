#ifndef DB_H
#define DB_H
#include <vector>
#include <iostream>
#include <boost/redis/connection.hpp>
#include <obelisk/database/mysql/mysql_connection.h>
#include <sahara/log/log.h>

#include "db_pool.h"
#include "builder/select_statement.h"
#include "builder/insert_statement.h"
#include "builder/update_statement.h"
#include "builder/delete_statement.h"
#include "builder/detail/query.h"
#include "migration/migration.h"

namespace obelisk::database
{
    class db {
    public:
        db(const db&) = delete;
        db& operator=(const db&) = delete;
        static db& getInstance() {
            static db instance;
            return instance;
        }

        static builder::select_statement select(const std::initializer_list<builder::detail::selectable_t>& selectables)
        {
            builder::select_statement select;
            select.select(selectables);
            return select;
        }

        static builder::insert_statement insert(const std::string& table)
        {
            builder::insert_statement statement{table};
            return statement;
        }
        static builder::insert_statement insert(const std::string& table, const std::vector<builder::detail::col>& cols)
        {
            builder::insert_statement statement{table, cols};
            return statement;
        }

        static builder::update_statement update(const std::vector<builder::detail::table>& tables)
        {
            builder::update_statement statement{tables};
            return statement;
        }

        static builder::delete_statement delete_from(const std::vector<builder::detail::table>& tables)
        {
            builder::delete_statement statement{tables};
            return statement;
        }

        static boost::asio::awaitable<void> transaction(std::function<boost::asio::awaitable<void> (std::shared_ptr<mysql_connection> connection)> func, const std::string& inst = "default")
        {
            const auto connection = co_await db_pool::get_connection<mysql_connection>(inst);
            co_await connection->co_query<void>("SET AUTOCOMMIT=0;");
            co_await connection->co_query<void>("START TRANSACTION;");
            std::optional<std::string> exceptional;
            try
            {
                co_await func(connection);
            }catch (std::exception& e)
            {
                exceptional = e.what();
            }
            if (exceptional.has_value())
                co_await connection->co_query<void>("ROLLBACK;");
            else
                co_await connection->co_query<void>("COMMIT;");
            co_await connection->co_query<void>("SET AUTOCOMMIT=1;");
            if (exceptional.has_value())
                throw std::logic_error(exceptional.value());
        }

        static boost::asio::awaitable<void> run_migration(std::vector<std::shared_ptr<migration::base_migration>> migrations, const std::string& inst = "default")
        {
            const auto prefix = http::config::get<std::string>("database.default.prefix", "");
            const auto connection = co_await db_pool::get_connection<mysql_connection>(inst);
            // Check if migrations table exists
            {
                auto query = std::format("SHOW TABLES LIKE '{}';", prefix + "migrations");
                boost::mysql::results results = co_await connection->co_query(query);
                if (results.rows().empty())
                {
                    migration::create_migration_table migration;
                    co_await migration.up();
                }
            }
            // Get Max Branch
            std::int64_t batch = 1;
            {
                auto query = std::format("select MAX(batch) from `{}`;", prefix+"migrations");
                boost::mysql::results results = co_await connection->co_query(query);
                if (!results.rows().empty())
                {
                    batch =   results.rows()[0][0].is_null()? 1:results.rows()[0][0].as_int64() + 1;
                }
            }
            for (auto &migration : migrations)
            {
                try
                {
                    auto migration_name = migration->migration_name();
                    auto query = std::format("SELECT id, batch, migration FROM `{}` WHERE migration='{}'", prefix + "migrations", migration_name);
                    if (boost::mysql::results results = co_await connection->co_query(query); results.rows().empty()){
                        LOG_TRACE("Running migration {} ...", migration_name);
                        co_await migration->up();
                        co_await insert("migrations").values({
                            {"migration", migration_name},
                            {"batch", batch}
                        }).get();
                        LOG_TRACE("Running migration {} complete!", migration->migration_name());
                    }
                }catch (std::exception& e)
                {
                    LOG_CRITICAL("{}", e.what());
                    break;
                }
            }
            co_return;
        }

    private:
        db() = default;
        ~db() = default;
    };

}

#endif // DB_H
