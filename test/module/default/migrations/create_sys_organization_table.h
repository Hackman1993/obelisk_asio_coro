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
                blueprint.string("description").nullable();
                blueprint.string("director_name").nullable().comment("负责人名称");
                blueprint.string("director_phone").nullable().comment("负责人电话");
                blueprint.string("emergency_name").nullable().comment("紧急联系人");
                blueprint.string("emergency_phone").nullable().comment("应急联系电话");
                blueprint.string("address").nullable().comment("联系地址");
                blueprint.foreign_id("parent_id").nullable().references("sys_organizations","id").comment("父级ID");
                blueprint.timestamps();
                blueprint.soft_delete();
            });

            co_await obelisk::database::migration::migration::create("sys_organization_hierarchy", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();

                blueprint.bigint("fn_ancestor_id", true).index().references("sys_organizations", "id", "fk_sys_organization_hierarchy_ancestor_id");
                blueprint.bigint("fn_descendant_id", true).index().references("sys_organizations", "id", "fk_sys_organization_hierarchy_descendant_id");
                blueprint.integer("path_length", "int", true);
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_await obelisk::database::db::insert("sys_organizations").values({
                {"name", "Root"}
            }).get();
            co_await obelisk::database::db::insert("sys_organization_hierarchy").values({
                {"fn_ancestor_id", 1},
                {"fn_descendant_id", 1},
                {"path_length", 0}
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
