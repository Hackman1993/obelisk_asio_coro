#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>
#include "controllers/auth_controller.h"
#include "controllers/attachment_controller.h"
#include "database/database.h"
#include "database/mysql/mysql_connection.h"
#include "http/parser/http_parser_v2.h"
#include <boost/cobalt.hpp>

#include "controllers/article_category.h"
#include "database/mongo/mongo_connection.h"
#include "database/redis/redis_connection.h"
#include "middleware/authorization.h"

boost::cobalt::main co_main(int argc, char* argv[]) {

    using namespace obelisk::database;
    try {
        sw::redis::ConnectionOptions options;
        boost::asio::io_context ioctx;
        connection_manager::make_pool<mysql::mysql_connection>(ioctx, "mysql", "root", "hl97005497--", "umami", "localhost");
        connection_manager::make_pool<redis::redis_connection>(ioctx, "redis", options);
        connection_manager::make_pool<mongo::mongo_connection>(ioctx, "mongo", "mongodb://root:hl97005497--@localhost:27017");

        http_server server(ioctx, R"(C:\Users\Hackman.Lo\Desktop\BBC)");
        server.before_middlewares(std::make_unique<authorization>());
        server.route("/auth/login", login)->method({"POST"});
        server.route("/api/attachment/upload", upload)->method({"POST"});
        server.route("/api/logout", logout)->method({"GET"});
        server.route("/api/article", create_article)->method({"GET"});
        server.listen("0.0.0.0", 3308);
        ioctx.run();
    }
    catch (std::exception&err) {
        std::cout << err.what() << std::endl;
    }
    co_return 0;
}
