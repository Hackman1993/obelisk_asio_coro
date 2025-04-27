//
// Created by hackman on 4/25/25.
//

#ifndef CREATE_SYS_ADMINS_TABLE_H
#define CREATE_SYS_ADMINS_TABLE_H
#include <obelisk/database/migration/migration.h>

class create_sys_admins_table : public obelisk::database::migration::base_migration
{
    DEFINE_OBELISK_MIGRATION
public:
    boost::asio::awaitable<void> up() override
    {
        obelisk::database::migration::migration::create("sys_admins", [](obelisk::database::migration::table_blueprint& table) {)
            table.id();
            table.string("username").unique();
            table.string("password");
            table.string("real_name").nullable();
            table.string("phone", 20).unique();
            table.timestamps();
            table.soft_delete();
        });
    };
    boost::asio::awaitable<void> down() override;
};

#endif //CREATE_SYS_ADMINS_TABLE_H
