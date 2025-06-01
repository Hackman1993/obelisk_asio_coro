//
// Created by hackman on 5/30/25.
//

#include "create_members_table.h"

namespace module::crown_plaza::migrations
{
    boost::asio::awaitable<void> create_members_table::up()
    {
        co_await obelisk::database::migration::migration::create("members", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("name", 100);
                blueprint.string("number", 50).nullable();
                blueprint.string("nationality", 50).nullable();
                blueprint.string("passport_no", 50).unique();
                blueprint.string("picture_path").nullable();
                blueprint.float_col("discount").default_value(1);
                blueprint.boolean("gender").default_value(true);
                blueprint.date("birthday").nullable();
                blueprint.string("phone", 100).nullable();
                blueprint.string("email", 50).nullable();
                blueprint.string("description").nullable();
                blueprint.date("passport_issue_at").nullable();
                blueprint.date("passport_expires_at").nullable();
                blueprint.date("visa_sign_at").nullable();
                blueprint.date("visa_expires_at").nullable();
                blueprint.timestamps();
                blueprint.soft_delete();
            });
        co_return;
    }

    boost::asio::awaitable<void> create_members_table::down()
    {
        co_return;
    }
}
