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
#include "middleware/backend_auth.h"
#include "migrations/insert_permissions_data.h"
namespace module {
    void default_module::route(obelisk::http::http_server& server)
    {
        server.route("/api/backend/login", default_::controllers::backend_login)->method({"POST"});
        server.route("/api/backend/logout", default_::controllers::backend_logout)->method({"PUT"}).middleware(default_::middleware::backend_auth("sys_admins",{}));
        server.route("/api/backend/admin/info", default_::controllers::backend_permissions)->method({"GET"}).middleware(default_::middleware::backend_auth("sys_admins",{}));

    }

    boost::asio::awaitable<void> default_module::migrate(){
        co_await obelisk::database::db::run_migration({
            std::make_shared<default_::migrations::create_sys_organization_table>(),
            std::make_shared<default_::migrations::create_sys_admin_table>(),
            std::make_shared<default_::migrations::create_sys_permissions_table>(),
            std::make_shared<default_::migrations::create_sys_access_tokens_table>(),
            std::make_shared<default_::migrations::create_sys_roles_table>(),
            std::make_shared<default_::migrations::create_sys_verify_codes_table>(),
            std::make_shared<default_::migrations::insert_permissions_data>()
        });
        co_return;
    }
} // module