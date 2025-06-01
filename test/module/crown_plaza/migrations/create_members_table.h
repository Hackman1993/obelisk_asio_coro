//
// Created by hackman on 5/30/25.
//

#ifndef CREATE_MEMBERS_TABLE_H
#define CREATE_MEMBERS_TABLE_H
#include "obelisk/database/migration/migration.h"

namespace module::crown_plaza::migrations
{

    class create_members_table : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override;
        boost::asio::awaitable<void> down() override;
    };

}

#endif //CREATE_MEMBERS_TABLE_H
