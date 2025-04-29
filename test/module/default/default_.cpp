//
// Created by hackman on 4/27/25.
//

#include "default_.h"

#include <obelisk/database/db.h>

#include "migrations/create_sys_access_tokens_table.h"
#include "migrations/create_sys_admin_table.h"
#include "migrations/create_sys_organization_table.h"
#include "migrations/create_sys_roles_table.h"

namespace module {
    void default_::route(obelisk::http::http_server& server) {}

    boost::asio::awaitable<void> default_::migrate(){
        co_await obelisk::database::db::run_migration({
            std::make_shared<::default_::migrations::create_sys_organization_table>(),
            std::make_shared<::default_::migrations::create_sys_admin_table>(),
            std::make_shared<::default_::migrations::create_sys_access_tokens_table>(),
            std::make_shared<::default_::migrations::create_sys_roles_table>()
        });
        co_return;
    }
} // module