#include <iostream>
#include <core/details/acceptor_base.h>
#include <http/http_server.h>
int main() {
    try{
        boost::asio::io_context ioctx;
        obelisk::http::http_server server(ioctx, "C:\\Users\\Hackman.Lo\\Desktop\\BBC");
        server.listen("127.0.0.1", 3308);
        ioctx.run();
    } catch (std::exception& err){
        std::cout << err.what() << std::endl;
    }
    return 0;
}
