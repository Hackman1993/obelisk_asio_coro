//
// Created by hackman on 5/1/25.
//

#ifndef INSERT_CROWN_PLAZA_PERMISSIONS_DATA_H
#define INSERT_CROWN_PLAZA_PERMISSIONS_DATA_H
#include <obelisk/database/migration/migration.h>
#include <module/default/utils/utils.h>
namespace module::crown_plaza::migrations
{
    using namespace obelisk::database;
    class insert_crown_plaza_permissions_data final : public migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await default_::utils::register_permission("permission.member.view", false);
            co_await default_::utils::register_permission("permission.member.create", false);
            co_await default_::utils::register_permission("permission.member.update", false);
            co_await default_::utils::register_permission("permission.member.delete", false);
            co_await default_::utils::register_permission("permission.member.enable", false);
            co_await default_::utils::register_permission("permission.member.assign_card", false);
            co_await default_::utils::register_permission("permission.member.freeze_card", false);
            co_await default_::utils::register_permission("permission.member.view_detail", false);
            co_await default_::utils::register_permission("permission.member.reset_card_password", false);
            co_await default_::utils::register_permission("permission.member_entrance_record.view", false);
            co_await default_::utils::register_permission("permission.sys_admin.assign_card", true);
            co_await default_::utils::register_permission("permission.statistics.member_region", false);


            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //INSERT_CROWN_PLAZA_PERMISSIONS_DATA_H
