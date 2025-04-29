//
// Created by hackman on 4/28/25.
//

#ifndef CREATE_SYS_ROLES_TABLE_H
#define CREATE_SYS_ROLES_TABLE_H
#include <obelisk/database/migration/migration.h>
namespace default_::migrations
{
    class create_sys_roles_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_roles", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("name", 50).unique();
                blueprint.foreign_id("fn_organization_id").index().references("sys_organizations", "id");
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_SYS_ROLES_TABLE_H
