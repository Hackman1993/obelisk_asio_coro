/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_server.h
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/

#ifndef OBELISK_HTTP_SERVER_H
#define OBELISK_HTTP_SERVER_H
#include <filesystem>

#include "http_connection.h"
#include "middleware/middleware.h"
#include "router/route_item.h"
#include "../core/coroutine/task.h"
#include <boost/asio.hpp>

#include "module/base_module.h"

namespace obelisk::http {
    struct http_header;
    class http_response;
    class http_request;
    class route_item;
    class http_middleware_base;

    typedef task<std::unique_ptr<http_response>>(*request_handler)(http_request_wrapper&);

    class http_server {
    public:
        http_server(boost::asio::io_context& ctx);

        void listen(const std::string& address, unsigned short port);
        std::unique_ptr<route_item>& route(const std::string& route, const std::function<obelisk::task<std::unique_ptr<http_response>> (http_request_wrapper&)>& handler);
        std::unique_ptr<route_item>& route(const std::string& route, const request_handler& handler);

        const std::vector<std::unique_ptr<middleware::after_middleware>>& after_middlewares();
        const std::vector<std::unique_ptr<middleware::before_middleware>>& before_middlewares();

        void after_middlewares(std::unique_ptr<middleware::after_middleware>);
        void before_middlewares(std::unique_ptr<middleware::before_middleware>);

        template<typename T>
        std::enable_if_t<std::is_base_of_v<module::base_module, T>, boost::asio::awaitable<void>>
        module(T module)
        {
            co_await module.migrate();
            module.route(*this);
            co_return;
        }
    protected:
        boost::asio::ip::tcp::acceptor acceptor_;
        std::vector<std::unique_ptr<route_item>> routes_;
        std::vector<std::unique_ptr<middleware::after_middleware>> middlewares_after_;
        std::vector<std::unique_ptr<middleware::before_middleware>> middlewares_before_;

        task<void> listen_();
        task<void> handle_(boost::asio::ip::tcp::socket socket);
        void handle_accept_(const boost::system::error_code& error, boost::asio::ip::tcp::socket socket);
        static task<http::core::raw::http_header_raw> receive_header_(boost::asio::ip::tcp::socket &socket, boost::asio::streambuf &buffer);
        static task<std::unique_ptr<std::iostream>> receive_body_(boost::asio::ip::tcp::socket &socket, boost::asio::streambuf& buffer, core::raw::http_header_raw& header);
        static task<void> write_response_(boost::asio::ip::tcp::socket& socket, const std::unique_ptr<core::http_iodata>& response);

        boost::asio::io_context& ioctx_;
    };

} // obelisk::http

#endif //OBELISK_HTTP_SERVER_H
