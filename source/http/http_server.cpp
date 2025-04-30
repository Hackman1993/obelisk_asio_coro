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
#include "obelisk/http/middleware/json_extract.h"
#include "obelisk/http/middleware/multipart_extract.h"


namespace obelisk::http {
    http_server::http_server(boost::asio::io_context&ctx) : acceptor_(ctx), ioctx_(ctx), signals_(ctx, SIGINT, SIGTERM) {
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
                auto header = co_await receive_header_(socket, buffer);;
                std::unique_ptr<std::iostream> body = co_await receive_body_(socket, buffer, header);

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
                std::unique_ptr<core::http_iodata> response_data = response->serialize();
                co_await write_response_(socket, response_data);
            }
        }
    }

    obelisk::task<http::core::raw::http_header_raw> http_server::receive_header_(
        boost::asio::ip::tcp::socket&socket, boost::asio::streambuf&buffer) {
        core::raw::http_header_raw header{};
        std::string_view bytes_view;
        do {
            const auto [ec, bytes_transferred] = co_await socket.async_read_some(
                buffer.prepare(1024 * 10), boost::asio::as_tuple(obelisk::use_token));
            buffer.commit(bytes_transferred);
            bytes_view = std::string_view(static_cast<const char *>(buffer.data().data()), buffer.size());
        }
        while (buffer.size() < 1024 * 10 && !bytes_view.contains("\r\n\r\n"));

        if (!bytes_view.contains("\r\n\r\n"))
            throw protocol_exception("Header Size Exceed, Shutting Down!");

        bytes_view = std::string_view(bytes_view.data(), bytes_view.find("\r\n\r\n") + 4);
        if (!parser_v3::parse_http_header(bytes_view, header))
            throw protocol_exception("Header Parse Failed, Shutting Down!");
        buffer.consume(bytes_view.size());
        co_return header;
    }

    obelisk::task<std::unique_ptr<std::iostream>> http_server::receive_body_(boost::asio::ip::tcp::socket&socket, boost::asio::streambuf&buffer, core::raw::http_header_raw&header) {
        if (!header.headers_.contains("Content-Length")) {
            co_return nullptr;
        }
        const auto content_length = std::stoul(header.headers_["Content-Length"]);
        if (content_length == 0) co_return nullptr;

        std::unique_ptr<std::iostream> ret;
        if (content_length < 1024 * 1024 * 1)
            ret = std::make_unique<std::stringstream>();
        else
            ret = std::make_unique<http_temp_fstream>("./" + sahara::utils::uuid::generate());

        uint32_t total_transferred = 0;
        if(buffer.size() > 0) {
            total_transferred = std::min<uint32_t>(buffer.size(), content_length);
            ret->write(static_cast<const char *>(buffer.data().data()), total_transferred);
            buffer.consume(total_transferred);
        }
        while (total_transferred < content_length) {
            const auto bytes_wanna_read = std::min<uint32_t>(content_length - total_transferred, 1024 * 10);
            const auto transferred = co_await socket.async_read_some(buffer.prepare(bytes_wanna_read), obelisk::use_token);
            buffer.commit(transferred);
            total_transferred += transferred;
            ret->write(static_cast<const char *>(buffer.data().data()), transferred);
            buffer.consume(transferred);
        }

        ret->flush();
        co_return ret;
    }

    obelisk::task<void> http_server::write_response_(boost::asio::ip::tcp::socket&socket, const std::unique_ptr<core::http_iodata>&response) {
        unsigned char buffer[1024 * 256] = {};
        while (!response->eof()) {
            const auto bytes_read = response->read(buffer, 1024 * 256);
            std::uint64_t bytes_transferred = 0;
            while (bytes_transferred< bytes_read){
                bytes_transferred += co_await socket.async_write_some(boost::asio::const_buffer(&buffer[bytes_transferred], bytes_read - bytes_transferred), obelisk::use_token);
            }
        }
        co_return;
    }
} // http::obelisk
