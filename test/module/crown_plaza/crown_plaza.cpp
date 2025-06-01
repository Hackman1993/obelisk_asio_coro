#include "crown_plaza.h"

#include "migrations/create_cards_table.h"
#include "migrations/create_members_table.h"
#include "migrations/create_member_entrance_records_table.h"
#include "migrations/insert_crown_plaza_permissions_data.h"
#include "obelisk/database/db.h"

namespace module
{
    void crown_plaza_module::route(obelisk::http::http_server& server)
    {
    }

    boost::asio::awaitable<void> crown_plaza_module::migrate()
    {
        co_await obelisk::database::db::run_migration({
            std::make_unique<crown_plaza::migrations::create_cards_table>(),
            std::make_unique<crown_plaza::migrations::create_members_table>(),
            std::make_unique<crown_plaza::migrations::create_member_entrance_records_table>(),
            std::make_unique<crown_plaza::migrations::insert_crown_plaza_permissions_data>()
        });

        co_return;
    }
} // module
