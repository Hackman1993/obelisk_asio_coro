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
        void start();
        std::unique_ptr<route_item>& route(const std::string& route, const std::function<obelisk::task<std::unique_ptr<http_response>> (http_request_wrapper&)>& handler);
        std::unique_ptr<route_item>& route(const std::string& route, const request_handler& handler);

        const std::vector<std::shared_ptr<middleware::base_middleware>>& middlewares();
        void reg_middlewares(const std::initializer_list<std::shared_ptr<middleware::base_middleware>>& middlewares);
        void module(std::initializer_list<std::unique_ptr<module::base_module>> modules)
        {
            boost::asio::co_spawn(ioctx_, [modules, this]()->boost::asio::awaitable<void>
            {
                for (auto& module: modules)
                {
                    co_await module->migrate();
                    module->route(*this);
                }
                co_return;
            },boost::asio::detached);
            ioctx_.run();
            ioctx_.restart();
        }
        task<void> listen_();
    protected:
        boost::asio::ip::tcp::acceptor acceptor_;
        std::vector<std::unique_ptr<route_item>> routes_;
        std::vector<std::shared_ptr<middleware::base_middleware>> middlewares_;


        task<void> handle_(boost::asio::ip::tcp::socket socket);
        void handle_accept_(const boost::system::error_code& error, boost::asio::ip::tcp::socket socket);

        boost::asio::io_context& ioctx_;
        boost::asio::signal_set signals_;
    };

} // obelisk::http

#endif //OBELISK_HTTP_SERVER_H
