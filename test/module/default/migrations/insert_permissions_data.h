//
// Created by hackman on 5/1/25.
//

#ifndef INSERT_PERMISSIONS_DATA_H
#define INSERT_PERMISSIONS_DATA_H
#include <obelisk/database/migration/migration.h>
namespace module::default_::migrations
{
    using namespace obelisk::database;
    class insert_permissions_data final : public migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await utils::register_permission("permission.sys_admin.view", true);
            co_await utils::register_permission("permission.sys_admin.update", true);
            co_await utils::register_permission("permission.sys_admin.create", true);
            co_await utils::register_permission("permission.sys_admin.delete", true);

            co_await utils::register_permission("permission.sys_role.view", true);
            co_await utils::register_permission("permission.sys_role.update", true);
            co_await utils::register_permission("permission.sys_role.create", true);
            co_await utils::register_permission("permission.sys_role.delete", true);


            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //INSERT_PERMISSIONS_DATA_H
