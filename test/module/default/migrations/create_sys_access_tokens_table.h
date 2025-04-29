//
// Created by hackman on 4/28/25.
//

#ifndef CREATE_SYS_ACCESS_TOKENS_TABLE_H
#define CREATE_SYS_ACCESS_TOKENS_TABLE_H

#include <obelisk/database/migration/migration.h>
namespace default_::migrations
{
    class create_sys_access_tokens_table : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_access_tokens", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("type", 50).index();
                blueprint.foreign_id("fn_target_id").index();
                blueprint.string("target_key", 100).index();
                blueprint.string("token", 64).unique();
                blueprint.timestamp("last_used_at").nullable();
                blueprint.timestamp("expires_at").nullable();
                blueprint.timestamps();
                blueprint.index({"fn_target_id", "target_key", "type"});
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_SYS_ACCESS_TOKENS_TABLE_H
