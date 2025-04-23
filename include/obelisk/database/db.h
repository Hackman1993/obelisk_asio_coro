#ifndef DB_H
#define DB_H
#include <vector>
#include <database/mysql/mysql_connection.h>
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

        boost::asio::awaitable<void> db::run_migration(std::vector<std::shared_ptr<migration::migration_base>> migrations)
        {
            for (auto &migration : migrations)
            {
                auto connection  = co_await db_pool::get_connection<mysql_connection>("mysql");
                LOG_TRACE("Running migration {} ...", migration->migration_name());
                connection->a
                LOG_TRACE("Running migration {} ...", migration->migration_name());
            }
        }

    private:
        db() = default;
        ~db() = default;
    };

}

#endif // DB_H
