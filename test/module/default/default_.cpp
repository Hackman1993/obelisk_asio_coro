//
// Created by hackman on 4/27/25.
//

#include "default_.h"

#include <obelisk/database/db.h>
#include <obelisk/http/http_server.h>

#include "controllers/auth_controller.h"
#include "controllers/sys_admin_controller.h"
#include "controllers/sys_organization_controller.h"
#include "controllers/sys_role.h"
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
        using namespace default_;
        server.route("/api/send_sms", controllers::send_sms)->method({"POST"});
        server.route("/api/backend/login", controllers::backend_login)->method({"POST"});
        server.route("/api/backend/logout", controllers::backend_logout)->method({"PUT"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/current/info", controllers::backend_user_info)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/current/permissions", controllers::backend_permissions)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));

        server.route("/api/backend/organization/create", controllers::sys_organization_controller::backend_create)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/organization/view", controllers::sys_organization_controller::backend_view)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/organization/update", controllers::sys_organization_controller::backend_update)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/organization/delete", controllers::sys_organization_controller::backend_delete)->method({"DELETE"}).middleware(middleware::backend_auth("sys_admins"));

        server.route("/api/backend/admin/view", controllers::sys_admin_controller::backend_view)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/admin/create", controllers::sys_admin_controller::backend_create)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/admin/update", controllers::sys_admin_controller::backend_update)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/admin/delete", controllers::sys_admin_controller::backend_delete)->method({"DELETE"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/admin/assignable_roles", controllers::sys_admin_controller::backend_assignable_role)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/admin/assign_roles", controllers::sys_admin_controller::backend_assign_role)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));

        server.route("/api/backend/role/view", controllers::sys_role::backend_view)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/role/create", controllers::sys_role::backend_create)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/role/update", controllers::sys_role::backend_update)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/role/delete", controllers::sys_role::backend_delete)->method({"DELETE"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/role/assignable_permissions", controllers::sys_role::backend_assignable_permission)->method({"GET"}).middleware(middleware::backend_auth("sys_admins"));
        server.route("/api/backend/role/assign_permissions", controllers::sys_role::backend_assign_permission)->method({"POST"}).middleware(middleware::backend_auth("sys_admins"));

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