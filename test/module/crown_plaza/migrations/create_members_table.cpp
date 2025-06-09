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
                blueprint.string("name", 100).index().nullable();
                blueprint.string("number", 50).nullable().index();
                blueprint.string("nationality", 50).nullable().index();
                blueprint.string("passport_no", 50).unique().nullable();
                blueprint.string("picture_path").nullable();
                blueprint.boolean("gender").default_value(true).index();
                blueprint.date("birthday").nullable().index();
                blueprint.string("phone", 100).nullable().index();
                blueprint.string("email", 50).nullable();
                blueprint.string("description").nullable();
                blueprint.date("passport_issue_at").nullable().index();
                blueprint.date("passport_expires_at").nullable().index();
                blueprint.date("visa_issue_at").nullable().index().index();
                blueprint.date("visa_expires_at").nullable().index().index();
                blueprint.string("passport_picture").nullable();
                blueprint.string("passport_sign_location", 100).nullable().index();
                blueprint.datetime("last_entry_at").nullable().index();
                blueprint.datetime("last_leave_at").nullable().index();
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
