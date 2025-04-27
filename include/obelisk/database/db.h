#ifndef DB_H
#define DB_H
#include <vector>
#include <iostream>
#include <obelisk/database/mysql/mysql_connection.h>
#include <sahara/log/log.h>

#include "db_pool.h"
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

        static boost::asio::awaitable<void> run_migration(std::vector<std::shared_ptr<migration::base_migration>> migrations)
        {
            const auto prefix = http::config::get<std::string>("database.default.prefix", "");
            const auto connection = co_await db_pool::get_connection<mysql_connection>("default");
            // Check if migrations table exists
            {
                auto query = boost::mysql::with_params("SHOW TABLES LIKE {};", prefix + "migrations");
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
                auto query = boost::mysql::with_params("select MAX(batch) from {:i};", prefix+"migrations");
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
                    auto query = boost::mysql::with_params("SELECT id, batch, migration FROM {:i} WHERE migration={}", prefix + "migrations", migration_name);
                    boost::mysql::results results = co_await connection->co_query(query);
                    if (results.rows().empty()){
                        LOG_TRACE("Running migration {} ...", migration_name);
                        co_await migration->up();
                        auto insert_query = boost::mysql::with_params("INSERT INTO `{:r}migrations` (migration, batch) VALUES ({}, {})", prefix, migration_name, batch);
                        co_await connection->co_execute(insert_query);
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
