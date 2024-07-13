#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>
#include "controllers/auth_controller.h"
#include "controllers/attachment_controller.h"
#include "database/database.h"
#include "database/mysql/mysql_connection.h"
#include "http/parser/http_parser_v2.h"

#include "controllers/article.h"
#include "controllers/article_category.h"
#include "database/mongo/mongo_connection.h"
#include "database/redis/redis_connection.h"
#include "middleware/authorization.h"
#include "middleware/cors.h"

boost::cobalt::main co_main(int argc, char* argv[]) {

    using namespace obelisk::database;
    try {
        sw::redis::ConnectionOptions options;
        boost::asio::io_context ioctx;
        connection_manager::make_pool<redis::redis_connection>(ioctx, "redis", options);
        connection_manager::make_pool<mongo::mongo_connection>(ioctx, "mongo", "mongodb://root:hl97005497--@localhost:27017");

        http_server server(ioctx, R"(C:\Users\Hackman.Lo\Desktop\BBC)");
        server.before_middlewares(std::make_unique<authorization>());
        server.after_middlewares(std::make_unique<cors>());
        server.route("/auth/login", login)->method({"POST"});
        server.route("/api/backend/check_auth", check_auth)->method({"GET"});
        server.route("/api/backend/attachment/upload", upload)->method({"POST"});
        server.route("/api/backend/logout", logout)->method({"GET"});

        server.route("/api/backend/article", article_controller::get_article_list)->method({"GET"});
        server.route("/api/article", article_controller::get_article_list)->method({"GET"});
        server.route("/api/article/{article_id}", article_controller::get_article_detail)->method({"GET"});
        server.route("/api/backend/article/create", article_controller::create_article)->method({"POST"});
        server.route("/api/backend/article/update/{article_id}", article_controller::update_article)->method({"PUT"});
        server.route("/api/backend/article/detail/{article_id}", article_controller::get_article_detail)->method({"GET"});
        server.route("/api/backend/article/delete/{article_id}", article_controller::delete_article)->method({"DELETE"});

        server.route("/api/backend/content/article_category", article_category::get_article_category_list)->method({"GET"});
        server.route("/api/backend/content/article_category/all", article_category::get_article_category_full)->method({"GET"});
        server.route("/api/article_category/all", article_category::get_article_category_full)->method({"GET"});
        server.route("/api/backend/content/article_category/create", article_category::create_article_category)->method({"POST"});
        server.route("/api/backend/content/article_category/update/{category_id}", article_category::update_article_category)->method({"PUT"});
        server.route("/api/backend/content/article_category/delete/{category_id}", article_category::delete_article_category)->method({"DELETE"});
        server.listen("0.0.0.0", 3308);
        ioctx.run();
    }
    catch (std::exception&err) {
        std::cout << err.what() << std::endl;
    }
    co_return 0;
}
