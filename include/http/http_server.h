/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_server.h
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H
#include "../core/acceptor.h"
#include <filesystem>
#include "http_connection.h"
#include "middleware/middleware.h"
#include "router/route_item.h"

namespace obelisk::http {
    struct http_header;
    class http_response;
    class http_request;
    class route_item;
    class http_middleware_base;
    class http_server {
    public:
        http_server(boost::asio::io_context& ctx, const std::string& webroot);

        void listen(const std::string& address, unsigned short port);
        std::unique_ptr<route_item>& route(const std::string& route, std::function<std::unique_ptr<http_response> (http_request_wrapper&)> handler);
    protected:
        boost::asio::ip::tcp::acceptor acceptor_;
        std::vector<std::unique_ptr<route_item>> routes_;
        std::vector<std::unique_ptr<middleware::after_middleware>> middlewares_after_;
        std::vector<std::unique_ptr<middleware::before_middleware>> middlewares_before_;

        boost::asio::awaitable<void> listen_();
        boost::asio::awaitable<void> handle_(boost::asio::ip::tcp::socket socket);
        boost::asio::awaitable<http::http_header> receive_header_(boost::asio::ip::tcp::socket &socket, boost::asio::streambuf &buffer);
        boost::asio::awaitable<std::unique_ptr<std::iostream>> receive_body_(boost::asio::ip::tcp::socket &socket, boost::asio::streambuf& buffer, http_header& header);
        boost::asio::awaitable<void> write_response_(boost::asio::ip::tcp::socket& socket, std::unique_ptr<core::http_iodata>& response);

        std::filesystem::path webroot_;
        std::vector<std::string> index_files_= {"index.html", "index.htm"};
    };

} // obelisk::http

#endif //OBELISK_HTTP_SERVER_H
