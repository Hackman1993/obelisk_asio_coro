//
// Created by hackman on 7/6/25.
//

#include "add_card_no_to_sys_admins.h"

namespace module {
namespace crown_plaza {
namespace migrations {
    boost::asio::awaitable<void> add_card_no_to_sys_admins::up()
    {
        co_await obelisk::database::migration::migration::table("sys_admins",[](obelisk::database::migration::table_blueprint& blueprint)
        {
            blueprint.string("card_no", 100).index().nullable();
        });
        co_return;
    }

    boost::asio::awaitable<void> add_card_no_to_sys_admins::down()
    {
        co_return;
    }
} // migrations
} // crown_plaza
} // module