//
// Created by hackman on 4/23/25.
//

#ifndef MIGRATION_H
#define MIGRATION_H

#include <functional>
#include <string>
#include "table_blueprint.h"
#include <obelisk/http/framework/config.h>
#include <obelisk/database/db_pool.h>
#define DEFINE_OBELISK_MIGRATION public: std::string migration_name() override { return __FILE_NAME__; }


namespace obelisk::database::migration
{
    class base_migration
    {
    public:
        virtual ~base_migration() = default;
        virtual std::string migration_name() = 0;

        virtual boost::asio::awaitable<void> up() = 0;
        virtual boost::asio::awaitable<void> down()=0;
    };


    class migration {
    public:
        migration(const migration&) = delete;
        migration& operator=(const migration&) = delete;
        static boost::asio::awaitable<void> create(const std::string& table, const std::function<void (table_blueprint&)>& call);

        static boost::asio::awaitable<void> table(const std::string& table, const std::function<void (table_blueprint&)>& call);

        static boost::asio::awaitable<void> drop_if_exists(const std::string&& table);

    private:
        static boost::asio::awaitable<table_blueprint> table_bp(const std::string& table, bool create, const std::function<void (table_blueprint&)>& call, const std::string& inst = "default");
        static migration& self();
        migration();
        ~migration() = default;

        std::string prefix_;
    };
    class create_migration_table : public base_migration
    {
        DEFINE_OBELISK_MIGRATION
        boost::asio::awaitable<void> up() override
        {
            co_await migration::create("migrations", [](table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("migration", 500).unique().comment("迁移名称");
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
