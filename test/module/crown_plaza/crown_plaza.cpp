#include "crown_plaza.h"

#include "controllers/cards.h"
#include "controllers/legacy.h"
#include "migrations/create_cards_table.h"
#include "migrations/create_members_table.h"
#include "migrations/create_member_entrance_records_table.h"
#include "migrations/insert_crown_plaza_permissions_data.h"
#include "obelisk/database/db.h"
#include "module/crown_plaza/controllers/member_controller.h"
#include "module/default/middleware/backend_auth.h"
#include "obelisk/http/http_server.h"

namespace module
{
    using namespace crown_plaza;
    void crown_plaza_module::route(obelisk::http::http_server& server)
    {
        server.route("/api/backend/member/view", controllers::member_controller::backend_view)->method({"GET"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/create", controllers::member_controller::backend_create)->method({"POST"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/update", controllers::member_controller::backend_update)->method({"POST"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/delete", controllers::member_controller::backend_delete)->method({"DELETE"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/freeze", controllers::member_controller::backend_freeze)->method({"PUT"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/sign_card", controllers::member_controller::backend_sign_card)->method({"POST"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/update_certificate_info", controllers::member_controller::backend_update_certificate_info)->method({"POST"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/backend/member/region_statistics", controllers::member_controller::region_statistics)->method({"GET"}).middleware(default_::middleware::backend_auth("sys_admins"));
        server.route("/api/member/entrance", controllers::member_controller::entrance)->method({"POST"});
        server.route("/api/member/find_by_card", controllers::cards::find_by_card)->method({"GET"});
        server.route("/api/terminal/get_token", controllers::legacy::get_terminal_token)->method({"POST"});
        server.route("/api/test", controllers::legacy::test)->method({"GET"});

    }

    boost::asio::awaitable<void> crown_plaza_module::migrate()
    {
        co_await obelisk::database::db::run_migration({
            std::make_unique<migrations::create_cards_table>(),
            std::make_unique<migrations::create_members_table>(),
            std::make_unique<migrations::create_member_entrance_records_table>(),
            std::make_unique<migrations::insert_crown_plaza_permissions_data>()
        });

        co_return;
    }
} // module
