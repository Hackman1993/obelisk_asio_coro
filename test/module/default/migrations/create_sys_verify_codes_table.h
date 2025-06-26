//
// Created by hackman on 4/29/25.
//

#ifndef CREATE_SYS_VERIFY_CODES_H
#define CREATE_SYS_VERIFY_CODES_H
#include <obelisk/database/migration/migration.h>
namespace module::default_::migrations
{
    class create_sys_verify_codes_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("sys_verify_codes", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("phone", 20).index();
                blueprint.string("code", 10).index();
                blueprint.string("type", 30).index();
                blueprint.timestamp("expires_at").index();
                blueprint.soft_delete();
                blueprint.timestamps();
                blueprint.index({"phone", "code", "type"});
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_SYS_VERIFY_CODES_H
