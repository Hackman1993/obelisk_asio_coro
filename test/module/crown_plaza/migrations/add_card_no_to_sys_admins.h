//
// Created by hackman on 7/6/25.
//

#ifndef ADD_CARD_NO_TO_SYS_ADMINS_H
#define ADD_CARD_NO_TO_SYS_ADMINS_H
#include "obelisk/database/migration/migration.h"


namespace module::crown_plaza::migrations {

class add_card_no_to_sys_admins : public obelisk::database::migration::base_migration {
    DEFINE_OBELISK_MIGRATION;
    boost::asio::awaitable<void> up() override;
    boost::asio::awaitable<void> down() override;
};

}

#endif //ADD_CARD_NO_TO_SYS_ADMINS_H
