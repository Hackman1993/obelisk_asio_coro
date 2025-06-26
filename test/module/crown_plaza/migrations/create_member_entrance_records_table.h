//
// Created by hackman on 5/30/25.
//

#ifndef CREATE_MEMBER_ENTRANCE_RECORDS_H
#define CREATE_MEMBER_ENTRANCE_RECORDS_H
#include "obelisk/database/migration/migration.h"

namespace module::crown_plaza::migrations
{

    class create_member_entrance_records_table : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override;
        boost::asio::awaitable<void> down() override;
    };

}

#endif //CREATE_MEMBER_ENTRANCE_RECORDS_H
