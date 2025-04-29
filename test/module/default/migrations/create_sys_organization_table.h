//
// Created by hackman on 4/27/25.
//

#ifndef CREATE_SYS_ORGANIZATION_TABLE_H
#define CREATE_SYS_ORGANIZATION_TABLE_H

#include <obelisk/database/db.h>
#include <obelisk/database/migration/migration.h>
namespace module::default_::migrations
{
    class create_sys_organization_table : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_organizations", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("name").unique();
                blueprint.string("director_name").nullable().comment("负责人名称");
                blueprint.string("director_phone").nullable().comment("负责人电话");
                blueprint.string("emergency_name").nullable().comment("紧急联系人");
                blueprint.string("emergency_phone").nullable().comment("应急联系电话");
                blueprint.string("address").nullable().comment("联系地址");
                blueprint.integer("_lft", "int", true).default_value(0);
                blueprint.integer("_rgt", "int", true).default_value(0);
                blueprint.integer("parent_id", "bigint", true).nullable();
                blueprint.index({"_lft", "_rgt", "parent_id"});
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_await obelisk::database::db::insert("sys_organizations").values({
                {"name", "Root"}
            }).get();
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}



#endif //CREATE_SYS_ORGANIZATION_TABLE_H
