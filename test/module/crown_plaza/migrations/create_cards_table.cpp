//
// Created by hackman on 5/30/25.
//

#include "create_cards_table.h"

namespace module::crown_plaza::migrations
{
    boost::asio::awaitable<void> create_cards_table::up()
    {
        co_await obelisk::database::migration::migration::create("cards", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("card_no", 100).index();
                blueprint.string("password", 100).nullable();
                blueprint.integer("card_type").nullable().index();
                blueprint.string("cardable_type", 50).index();
                blueprint.foreign_id("fn_cardable_id").index();
                blueprint.foreign_id("fn_creator_id").references("sys_admins", "id");
                blueprint.foreign_id("fn_issuer_id").references("sys_admins", "id");
                blueprint.timestamps();
                blueprint.soft_delete();
            });
        co_return;
    }

    boost::asio::awaitable<void> create_cards_table::down()
    {
        co_return;
    }
}
