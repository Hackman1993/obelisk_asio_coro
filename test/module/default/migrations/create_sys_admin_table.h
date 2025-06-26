//
// Created by hackman on 4/27/25.
//

#ifndef CREATE_SYS_ADMIN_TABLE_H
#define CREATE_SYS_ADMIN_TABLE_H

#include <obelisk/database/db.h>
#include <obelisk/database/migration/migration.h>
#include <sahara/hash/bcrypt.h>
namespace module::default_::migrations
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
                blueprint.string("phone", 20).unique().nullable();
                blueprint.string("passport_no", 50).nullable();
                blueprint.string("email", 50).nullable();
                blueprint.boolean("status").default_value(true).comment("0: Disabled 1: Enabled");
                blueprint.string("avatar_url").nullable();
                blueprint.foreign_id("fn_organization_id").references("sys_organizations", "id");
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_await obelisk::database::db::insert("sys_admins").values({
                {"username", "admin"},
                {"password", sahara::hash::bcrypt::generateHash(std::string("123456"))},
                {"phone", "13658834664"},
                {"fn_organization_id", 1}
            }).get();
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        };
    };
}


#endif //CREATE_SYS_ADMIN_TABLE_H
