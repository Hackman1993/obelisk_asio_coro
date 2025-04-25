//
// Created by hackman on 4/23/25.
//

#ifndef MIGRATION_H
#define MIGRATION_H

#include <functional>
#include <string>
#include "table_blueprint.h"
#include <obelisk/http/framework/config.h>
#define DEFINE_OBELISK_MIGRATION public: std::string migration_name() override { return __FILE_NAME__; }


namespace obelisk::database::migration
{
    class migration_base
    {
    public:
        virtual ~migration_base() = default;
        virtual std::string migration_name() = 0;

        virtual boost::asio::awaitable<void> up() = 0;
        virtual boost::asio::awaitable<void> down()=0;
    };


    class migration {
    public:
        migration(const migration&) = delete;
        migration& operator=(const migration&) = delete;
        static boost::asio::awaitable<void> create(const std::string& table, const std::function<void (table_blueprint&)>& call)
        {
            table_blueprint bp = co_await table_bp(table, true, call);
            co_return;
        }

        static boost::asio::awaitable<void> table(const std::string& table, const std::function<void (table_blueprint&)>& call)
        {
            table_blueprint blueprint = co_await table_bp(table, false, call);
            co_return;
        }

        static boost::asio::awaitable<void> drop_if_exists(const std::string&& table)
        {
            co_return;
        }

    private:
        static boost::asio::awaitable<table_blueprint> table_bp(const std::string& table, const bool create, const std::function<void (table_blueprint&)>& call)
        {
            table_blueprint blueprint(table, create, self().prefix_);
            call(blueprint);
            const auto commands = blueprint.describe();

            const auto connection  = co_await db_pool::get_connection<mysql_connection>("mysql");
            for (auto &cmd: commands)
            {
                try{
                    boost::mysql::results result = co_await connection->co_execute(cmd.sql());
                }catch (std::exception &e)
                {
                    if (cmd.required())
                        throw;
                }
            }
            co_return blueprint;
        }
        static migration& self() {
            static migration instance;
            return instance;
        }
        migration()
        {
            prefix_ = http::config::get<std::string>("database.default.prefix", "t_");
        };
        ~migration() = default;

        std::string prefix_;
    };
    class create_migration_table : public migration_base
    {
        DEFINE_OBELISK_MIGRATION
        boost::asio::awaitable<void> up() override
        {
            co_await migration::create("migrations", [](table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("migration", 50).unique().comment("迁移名称");
                blueprint.integer("batch").comment("批次");
                blueprint.timestamps();
            });
            co_return;
        }
        boost::asio::awaitable<void> down() override
        {
            co_await migration::drop_if_exists("migrations");
            co_return;
        }
    };

}





#endif //MIGRATION_H
