//#include <iostream>
// #include <awaitables/async_mutex.hpp>
// #include <obelisk/obelisk.h>
// #include "controllers/auth_controller.h"
// #include "controllers/attachment_controller.h"
// #include <boost/cobalt.hpp>
// #include <obelisk/database/database.h>
//
// #include "common/global_configuration.h"
// #include "controllers/article.h"
// #include "controllers/article_category.h"
// #include "controllers/member_controller.h"
// #include "controllers/user_controller.h"
// #include "middleware/cors.h"
//
#include "obelisk/database/mysql/mysql_connection.h"
//
// #include "database/redis/redis_connection.h"
// #include <boost/mysql.hpp>
// #include <sahara/log/log.h>
#include <boost/parser/parser.hpp>
#include <obelisk/http/core/raw.h>
#include <boost/asio.hpp>
#include <controllers/auth_controller.h>
#include <controllers/member_controller.h>
#include <controllers/user_controller.h>
#include <middleware/cors.h>
#include <obelisk/database/database.h>
#include <sahara/log/log.h>
#include <obelisk/database/database.h>
#include <obelisk/http/framework.h>
#include <obelisk/core/coroutine/async_mutex.h>
using namespace  boost::parser;

int main(int argc, char* argv[]) {

    try {

        sahara::log::initialize();
        boost::asio::io_context ioctx;
        framework::init(ioctx);

#ifdef NDEBUG
        obelisk::database::db_pool::make_pool<mysql_connection>(ioctx, "mysql", "localhost", 3306, "root", "hl97005497--", "shop_test");
#else
        obelisk::database::db_pool::make_pool<mysql_connection>(ioctx, "mysql", "127.0.0.1", 3306, "root", "hl97005497--", "obelisk");
#endif
        co_spawn(ioctx, obelisk::database::db::run_migration({}), boost::asio::detached);
        http_server server(ioctx);
        server.after_middlewares(std::make_unique<cors>());
        server.route("/auth/login1", auth_controller::login1)->method({"POST"});
        server.route("/auth/login", auth_controller::login)->method({"POST"});
        server.route("/api/backend/check_auth", auth_controller::check_auth)->method({"GET"});
        server.route("/api/backend/permission", auth_controller::get_permissions)->method({"GET"});
        server.route("/api/backend/operator", user_controller::view)->method({"GET"});
        server.route("/api/backend/member", member_controller::view)->method({"GET"});

        server.route("/api/backend/member/create", member_controller::create)->method({"POST"});
        server.route("/api/backend/member/update", member_controller::update)->method({"POST"});
        server.route("/api/backend/member/freeze", member_controller::freeze)->method({"PUT"});
        server.route("/api/backend/member/sign_card", member_controller::sign_card)->method({"POST"});
        server.route("/api/backend/member/delete", member_controller::soft_delete)->method({"DELETE"});

        // OLD
        server.route("/api/terminal/get_token", controller_base::getTerminalToken)->method({"POST"});
        server.route("/api/member/find_by_precise_data", member_controller::findByPreciseData)->method({"GET"});
        server.route("/api/member/findby_card", member_controller::getCardOwner)->method({"GET"});
        server.route("/api/member/entrance", member_controller::entrance)->method({"POST"});
        server.listen("0.0.0.0", 3308);

        std::vector<std::shared_ptr<std::thread>> threads;

        ioctx.run();
     }
    catch (boost::mysql::error_with_diagnostics & err)
    {
        std::cout << err.get_diagnostics().client_message() << std::endl;
        std::cout << err.get_diagnostics().server_message() << std::endl;
    }
    catch (std::exception&err) {
        std::cout << err.what() << std::endl;
    }

    return 0;
}
