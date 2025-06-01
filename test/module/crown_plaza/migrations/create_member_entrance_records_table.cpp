//
// Created by hackman on 5/30/25.
//

#include "create_member_entrance_records_table.h"

namespace module::crown_plaza::migrations
{
    boost::asio::awaitable<void> create_member_entrance_records_table::up()
    {
        co_await obelisk::database::migration::migration::create("member_entrance_records", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.foreign_id("fn_member_id").references("members", "id").index();
                blueprint.boolean("entrance").default_value(1).index();
                blueprint.timestamp("occur_at").use_current();
                blueprint.timestamps();
                blueprint.soft_delete();
            });
        co_return;
    }

    boost::asio::awaitable<void> create_member_entrance_records_table::down()
    {
        co_return;
    }
}
