//
// Created by hackman on 4/30/25.
//

#ifndef CREATE_SYS_MID_ADMIN_ROLE_TABLE_H
#define CREATE_SYS_MID_ADMIN_ROLE_TABLE_H
#include <obelisk/database/migration/migration.h>

namespace module::default_::migrations
{
    class create_sys_mid_admin_role_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_mid_admin_role", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.foreign_id("fn_role_id").references("sys_roles", "id");
                blueprint.foreign_id("fn_admin_id").references("sys_admins", "id");
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_SYS_MID_ADMIN_ROLE_TABLE_H
