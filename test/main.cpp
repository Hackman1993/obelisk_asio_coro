#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>

#include "http/response/json_response.h"
#include "controllers/auth_controller.h"
int main() {
    try{
        boost::asio::io_context ioctx;
        obelisk::http::http_server server(ioctx, R"(C:\Users\Hackman.Lo\Desktop\BBC)");
        server.route("/bac", login)->method({"GET"});
        server.listen("0.0.0.0", 3308);
        ioctx.run();
    } catch (std::exception& err){
        std::cout << err.what() << std::endl;
    }
    return 0;
}
