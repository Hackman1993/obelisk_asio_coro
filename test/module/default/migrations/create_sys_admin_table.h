//
// Created by hackman on 4/27/25.
//

#ifndef CREATE_SYS_ADMIN_TABLE_H
#define CREATE_SYS_ADMIN_TABLE_H

#include <obelisk/database/migration/migration.h>

namespace default_::migrations
{
    class create_sys_admin_table : public obelisk::database::migration::base_migration
    {
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_admins", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("username", 20).unique();
                blueprint.string("password", 100).nullable();
                blueprint.string("real_name", 20).nullable();
                blueprint.string("phone", 20).unique();
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        };
    };
}


#endif //CREATE_SYS_ADMIN_TABLE_H
