#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>

#include "http/response/json_response.h"

int main() {
    try{

        boost::asio::io_context ioctx;
        obelisk::http::http_server server(ioctx, "C:\\Users\\Hackman.Lo\\Desktop\\BBC");
        server.route("/bac", [&](auto & request) {
            return std::make_unique<obelisk::http::json_response>(boost::json::object{
                {"data", nullptr}
            }, obelisk::http::EResponseCode::EST_OK);
        })->method({"GET"});
        server.listen("127.0.0.1", 3308);
        ioctx.run();
    } catch (std::exception& err){
        std::cout << err.what() << std::endl;
    }
    return 0;
}
