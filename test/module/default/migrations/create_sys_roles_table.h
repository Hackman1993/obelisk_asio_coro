//
// Created by hackman on 4/28/25.
//

#ifndef CREATE_SYS_ROLES_TABLE_H
#define CREATE_SYS_ROLES_TABLE_H
#include <obelisk/database/migration/migration.h>
namespace module::default_::migrations
{
    class create_sys_roles_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            using namespace obelisk::database;
            co_await migration::migration::create("sys_roles", [](migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("name", 50).unique();
                blueprint.string("description").nullable();
                blueprint.foreign_id("fn_organization_id").index().references("sys_organizations", "id");
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_await migration::migration::create("sys_mid_admin_role", [](migration::table_blueprint& blueprint)
            {
                blueprint.foreign_id("fn_role_id").references("sys_roles", "id");
                blueprint.foreign_id("fn_admin_id").references("sys_admins", "id");
                blueprint.index({"fn_admin_id", "fn_role_id"}, true);
            });

            co_await migration::migration::create("sys_mid_role_permission", [](migration::table_blueprint& blueprint)
            {
                blueprint.foreign_id("fn_role_id").references("sys_roles", "id");
                blueprint.foreign_id("fn_permission_id").references("sys_permissions", "id");
                blueprint.boolean("cascade");
                blueprint.boolean("grant");
                blueprint.index({"fn_permission_id", "fn_role_id"}, true);
            });
            co_await db::insert("sys_roles").values({
                {"name", "Super Admin"},
                {"fn_organization_id", 1}
            }).get();
            co_await db::insert("sys_mid_admin_role").values({
                {"fn_admin_id", 1},
                {"fn_role_id", 1}
            }).get();
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_SYS_ROLES_TABLE_H
