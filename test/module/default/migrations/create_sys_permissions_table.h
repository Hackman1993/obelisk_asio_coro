//
// Created by hackman on 4/30/25.
//

#ifndef CREATE_SYS_PERMISSIONS_TABLE_H
#define CREATE_SYS_PERMISSIONS_TABLE_H
#include <obelisk/database/migration/migration.h>

namespace module::default_::migrations
{
    class create_sys_permissions_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_permissions", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("code").unique();
                blueprint.string("visible").default_value(false);
                blueprint.timestamps();
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_SYS_PERMISSIONS_TABLE_H
