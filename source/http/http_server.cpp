/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_server.cpp
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/

#include <iostream>
#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "obelisk/http/http_server.h"

#include <obelisk/obelisk.h>
#include <boost/cobalt/spawn.hpp>
#include <obelisk/http/parser/http_parser_v3.h>
#include <sahara/log/log.h>
#include <nlohmann/json.hpp>
#include "obelisk/http/router/route_item.h"
#include "obelisk/http/core/http_request.h"
#include "obelisk/http/parser/http_parser_v2.h"
#include "obelisk/http/middleware/middleware.h"
#include "obelisk/http/response/json_response.h"
#include "obelisk/http/response/file_response.h"
#include "obelisk/http/response/empty_response.h"
#include "obelisk/http/exception/http_exception.h"
#include "obelisk/http/exception/protocol_exception.h"
#include "obelisk/http/middleware/url_params_extract.h"
#include "obelisk/http/core/http_iodata_stream_wrapper.h"
#include "obelisk/http/core/io.h"
#include "obelisk/http/middleware/json_extract.h"
#include "obelisk/http/middleware/multipart_extract.h"


namespace obelisk::http {
    http_server::http_server(boost::asio::io_context&ctx) : acceptor_(ctx), ioctx_(ctx), signals_(ctx, SIGINT, SIGTERM, SIGABRT) {
        reg_middleware(std::make_unique<middleware::url_params_extract>());
        reg_middleware(std::make_unique<middleware::multipart_extract>());
        reg_middleware(std::make_unique<middleware::json_extract>());
    }

    std::unique_ptr<route_item>& http_server::route(const std::string& route, const std::function<obelisk::task<std::unique_ptr<http_response>> (http_request_wrapper &)>& handler){
        return routes_.emplace_back(std::make_unique<route_item>(route, handler));
    }

    std::unique_ptr<route_item>& http_server::route(const std::string& route, const request_handler& handler) {
        return routes_.emplace_back(std::make_unique<route_item>(route, handler));
    }

    const std::vector<std::unique_ptr<middleware::base_middleware>>& http_server::middlewares() {
        return middlewares_;
    }

    void http_server::reg_middleware(std::unique_ptr<middleware::base_middleware> middleware) {
        middlewares_.push_back(std::move(middleware));
    }

    void http_server::listen(const std::string&address, unsigned short port) {
        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(address), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.bind(endpoint);
        acceptor_.listen();
        boost::asio::co_spawn(acceptor_.get_executor(), listen_(), boost::asio::detached);
    }

    obelisk::task<void> http_server::listen_() {
        signals_.async_wait([&](const boost::system::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Received signal: " << signal_number << " (Control+C or termination)" << std::endl;
                ioctx_.stop(); // 停止 io_context
            } else {
                std::cerr << "Error in signal handling: " << ec.message() << std::endl;
            }
        });
        while (true) {
            auto [ec, socket] = co_await acceptor_.async_accept(boost::asio::as_tuple(boost::asio::use_awaitable));
            boost::asio::co_spawn(ioctx_, handle_(std::move(socket)), boost::asio::detached);
        }
    }

    obelisk::task<void> http_server::handle_(boost::asio::ip::tcp::socket socket) {
        boost::asio::streambuf buffer;
        while (true) {
            std::unique_ptr<http_response> response;
            std::unique_ptr<http_request_wrapper> request;
            try {
                auto header = co_await core::io::receive_header_(socket, buffer);;
                std::unique_ptr<std::iostream> body = co_await core::io::receive_body_(socket, buffer, header);

                // Running Middleware
                request = std::make_unique<http_request_wrapper>(ioctx_, header, std::move(body));
                for (auto&before_middleware: middlewares_) {
                    response = co_await before_middleware->pre_handle(*request);
                    if (response) break;
                }
                // Matching Routes
                if (!response) {
                    for (const auto&ptr: routes_) {
                        std::unordered_map<std::string, std::string> route_params;
                        // If Route Not Match
                        if (!ptr->match(std::string(request->target()), route_params))
                            continue;
                        // If Method is Not Allowed
                        if (!ptr->method_allowed(request->method())) {
                            throw http_exception("Obelisk: Method Not Allowed!", EResponseCode::EST_METHOD_NOT_ALLOWED);
                        }
                        for(auto const &[key, val]: route_params) {
                            request->params().emplace(key, val);
                        }

                        // If Method is OPTIONS OR HEAD
                        if(request->method() == "OPTIONS" || request->method() == "HEAD") {
                            response = std::make_unique<empty_response>();
                            if(const std::string method_allowed = ptr->allowed_methods(); !method_allowed.empty()){
                                response->headers().emplace("Allow", method_allowed);
                                response->headers().emplace("Access-Control-Allow-Methods", method_allowed);
                            }
                        }
                        // Calling Handler
                        else {
                            auto& middlewares =ptr->middlewares();
                            for (const auto & route_middleware: middlewares)
                            {
                                co_await route_middleware->pre_handle(*request);
                            }
                            response = co_await ptr->handle(*request);
                            for (const auto & route_middleware: middlewares)
                            {
                                co_await route_middleware->after_handle(*request, *response);
                            }
                        }

                        if (response) break;
                    }
                }  // -- End Matching Routes --
            }
            // Catch System IO Error
            catch (const boost::system::system_error&e) {
                // If Error is EOF or Timeout
                if(e.code().value() == 2 || e.code().value() == 10053) {
                    response = nullptr;
                    socket.close();
                    co_return;
                }
                response = std::make_unique<json_response>(nlohmann::json{
                    {"code", 500},
                    {"message", "system.error.internal_server_error"}
                });
                LOG_MODULE_ERROR("Obelisk", "{}" , e.what());
            }
            // Catch Http Exception
            catch (const http_exception &e) {
                response = std::make_unique<json_response>(nlohmann::json{{"message", std::string(e.what())}}, e.code());
            }catch (const std::exception&e) {
                std::cout << e.what() << std::endl;
                response = std::make_unique<json_response>(nlohmann::json{{"message", std::string(e.what())}}, EST_INTERNAL_SERVER_ERROR);
            }

            // Generate 404 Response
            if (!response) {
                response = std::make_unique<json_response>(nlohmann::json{{"message", "null"}}, EResponseCode::EST_NOT_FOUND);
            }

            // Matching Response Middleware
            if (response) {
                for (auto&middleware: middlewares_) {
                    co_await middleware->after_handle(*request, *response);
                }
                std::unique_ptr<core::base_iodata> response_data = response->serialize();
                co_await core::io::write_data_(socket, response_data);
            }
        }
    }
} // http::obelisk
