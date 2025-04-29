//
// Created by hackman on 4/27/25.
//

#include "default_.h"

#include <obelisk/database/db.h>
#include <obelisk/http/http_server.h>

#include "controllers/auth_controller.h"
#include "migrations/create_sys_access_tokens_table.h"
#include "migrations/create_sys_admin_table.h"
#include "migrations/create_sys_organization_table.h"
#include "migrations/create_sys_roles_table.h"
#include "migrations/create_sys_verify_codes_table.h"
#include "migrations/create_sys_permissions_table.h"
#include "migrations/create_sys_mid_admin_role_table.h"
#include "migrations/create_sys_mid_role_permission_table.h"

namespace module {
    void default_module::route(obelisk::http::http_server& server)
    {
        server.route("/api/backend/login", ::default_::controllers::login)->method({"POST"});
    }

    boost::asio::awaitable<void> default_module::migrate(){
        co_await obelisk::database::db::run_migration({
            std::make_shared<default_::migrations::create_sys_organization_table>(),
            std::make_shared<default_::migrations::create_sys_admin_table>(),
            std::make_shared<default_::migrations::create_sys_access_tokens_table>(),
            std::make_shared<default_::migrations::create_sys_roles_table>(),
            std::make_shared<default_::migrations::create_sys_verify_codes_table>(),
            std::make_shared<default_::migrations::create_sys_permissions_table>(),
            std::make_shared<default_::migrations::create_sys_mid_admin_role_table>(),
            std::make_shared<default_::migrations::create_sys_mid_role_permission_table>()
        });
        co_return;
    }
} // module