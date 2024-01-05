#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>

#include "http/response/json_response.h"
#include "controllers/auth_controller.h"
#include "database/database.h"
#include "database/mysql/mysql_connection.h"
#include "http/parser/http_parser_v2.h"

int main() {
    try {
        boost::asio::io_context ioctx;
        obelisk::database::connection_manager::make_pool<obelisk::database::mysql::mysql_connection>(ioctx, "mysql", "root", "hl97005497--", "umami", "localhost");



        boost::mysql::tcp_connection connection(ioctx);
        obelisk::http::http_server server(ioctx, R"(C:\Users\Hackman.Lo\Desktop\BBC)");
        server.route("/bac", login)->method({"GET"});
        server.listen("0.0.0.0", 3308);
        // std::vector<std::thread> thread;
        // for (int i = 0; i < 10; i++) {
        //     thread.emplace_back([&] {
        //         ioctx.run();
        //     });
        // }
        ioctx.run();
    }
    catch (std::exception&err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
