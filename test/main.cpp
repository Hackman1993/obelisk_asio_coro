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
#include <boost/parser/parser.hpp>
#include <obelisk/http/core/raw.h>
#include <boost/asio.hpp>
#include <controllers/auth_controller.h>
#include <controllers/member_controller.h>
#include <controllers/user_controller.h>
#include <middleware/cors.h>
#include <module/default/controllers/auth_controller.h>
#include <module/default/controllers/auth_controller.h>
#include <module/default/controllers/auth_controller.h>
#include <module/default/controllers/auth_controller.h>
#include <obelisk/database/database.h>
#include <sahara/log/log.h>
#include <obelisk/http/framework.h>
#include <obelisk/core/coroutine/async_mutex.h>
#include <obelisk/http/core/base_client.h>

#include "clients/aliyun_oss_client.h"
#include "clients/aliyun_sms_client.h"
#include "module/default/default_.h"
#include "obelisk/http/client/request.h"
#include <sahara/hash/bcrypt.h>
using namespace  boost::parser;

int main(int argc, char* argv[]) {
    try {
        sahara::log::initialize();
        boost::asio::io_context ioctx;
        obelisk::http::framework::init(ioctx);

        obelisk::http::http_server server(ioctx);
        boost::asio::co_spawn(ioctx,server.module(module::default_module{}) ,boost::asio::detached);
        server.reg_middleware(std::make_unique<middleware::cors>());

        server.route("/auth/login", controller::auth_controller::login)->method({"POST"});
        server.route("/api/backend/check_auth", controller::auth_controller::check_auth)->method({"GET"});
        server.route("/api/backend/permission", controller::auth_controller::get_permissions)->method({"GET"});
        server.route("/api/backend/operator", user_controller::view)->method({"GET"});
        server.route("/api/backend/member", controller::member_controller::view)->method({"GET"});

        server.route("/api/backend/member/create", controller::member_controller::create)->method({"POST"});
        server.route("/api/backend/member/update", controller::member_controller::update)->method({"POST"});
        server.route("/api/backend/member/freeze", controller::member_controller::freeze)->method({"PUT"});
        server.route("/api/backend/member/sign_card", controller::member_controller::sign_card)->method({"POST"});
        server.route("/api/backend/member/delete", controller::member_controller::soft_delete)->method({"DELETE"});

        // OLD
        server.route("/api/terminal/get_token", controller_base::getTerminalToken)->method({"POST"});
        server.route("/api/member/find_by_precise_data", controller::member_controller::findByPreciseData)->method({"GET"});
        server.route("/api/member/findby_card", controller::member_controller::getCardOwner)->method({"GET"});
        server.route("/api/member/entrance", controller::member_controller::entrance)->method({"POST"});
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
