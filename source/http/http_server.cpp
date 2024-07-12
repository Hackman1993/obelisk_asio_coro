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

#include "http/http_server.h"

#include <boost/cobalt/config.hpp>
#include <boost/cobalt/spawn.hpp>

#include "http/router/route_item.h"
#include "http/core/http_request.h"
#include "http/parser/http_parser_v2.h"
#include "http/middleware/middleware.h"
#include "http/response/json_response.h"
#include "http/response/file_response.h"
#include "http/response/empty_response.h"
#include "http/exception/http_exception.h"
#include "http/exception/protocol_exception.h"
#include "http/middleware/url_params_extract.h"
#include "http/core/http_iodata_stream_wrapper.h"
#include "http/middleware/json_extract.h"
#include "http/middleware/multipart_extract.h"

namespace obelisk::http {
    http_server::http_server(boost::asio::io_context&ctx, const std::string&webroot) : acceptor_(ctx), webroot_(webroot), ioctx_(ctx) {
        if (!(std::filesystem::exists(webroot_) && std::filesystem::is_directory(webroot_)))
            throw std::logic_error("Root Dirctory Not Exists");
        before_middlewares(std::make_unique<middleware::url_params_extract>());
        before_middlewares(std::make_unique<middleware::multipart_extract>());
        before_middlewares(std::make_unique<middleware::json_extract>());
    }

    std::unique_ptr<route_item>& http_server::route(const std::string& route, const std::function<boost::asio::awaitable<std::unique_ptr<http_response>> (http_request_wrapper &)>& handler){
        return routes_.emplace_back(std::make_unique<route_item>(route, handler));
    }

    std::unique_ptr<route_item>& http_server::route(const std::string& route, const request_handler& handler) {
        return routes_.emplace_back(std::make_unique<route_item>(route, handler));
    }

    const std::vector<std::unique_ptr<middleware::after_middleware>>& http_server::after_middlewares() {
        return middlewares_after_;
    }

    const std::vector<std::unique_ptr<middleware::before_middleware>>& http_server::before_middlewares() {
        return middlewares_before_;
    }

    void http_server::after_middlewares(std::unique_ptr<middleware::after_middleware> middleware) {
        middlewares_after_.push_back(std::move(middleware));
    }

    void http_server::before_middlewares(std::unique_ptr<middleware::before_middleware> middleware) {
        middlewares_before_.push_back(std::move(middleware));
    }

    void http_server::listen(const std::string&address, unsigned short port) {
        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(address), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.bind(endpoint);
        acceptor_.listen();
        boost::asio::co_spawn(acceptor_.get_executor(), listen_(), boost::asio::detached);
    }

    boost::asio::awaitable<void> http_server::listen_() {
        while (true) {
            auto socket = co_await acceptor_.async_accept(boost::asio::use_awaitable);
            boost::asio::co_spawn(this->acceptor_.get_executor(), handle_(std::move(socket)), boost::asio::detached);
        }
    }

    boost::asio::awaitable<void> http_server::handle_(boost::asio::ip::tcp::socket socket) {
        boost::asio::streambuf buffer;
        while (true) {
            std::unique_ptr<http_response> response;
            std::unique_ptr<http_request_wrapper> request;
            try {
                auto header = co_await receive_header_(socket, buffer);;
                std::unique_ptr<std::iostream> body = co_await receive_body_(socket, buffer, header);
                request = std::make_unique<http_request_wrapper>(ioctx_, header, std::move(body));
                for (auto&before_middleware: middlewares_before_) {
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
                            response = co_await ptr->handle(*request);
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
                std::cout << e.what() << std::endl;
            }
            // Catch Http Exception
            catch (const http_exception &e) {
                response = std::make_unique<json_response>(boost::json::object{{"message", std::string(e.what())}}, e.code());
            }catch (const std::exception&e) {
                std::cout << e.what() << std::endl;
            }

            // Matching Static Files
            if (!response && request) {
                // Pretreatment Path, Prevent RCE Attach
                std::string target_path = webroot_.string() + "/" + std::string(request->target());
                boost::algorithm::replace_all(target_path, "\\", "/");
                boost::algorithm::replace_all(target_path, "..", "");
                do {
                    boost::algorithm::replace_all(target_path, "//", "/");
                }while (target_path.contains("//"));

                std::filesystem::path file_path(target_path);
                // Test If the file Exists
                if (std::filesystem::is_regular_file(file_path)) {
                    response = std::make_unique<file_response>(file_path, EResponseCode::EST_OK);
                }
                // Hittest Index Files
                for (auto&hittest: index_files_) {
                    if (std::filesystem::path index_path = file_path.string()+ "/" + hittest; std::filesystem::is_regular_file(index_path)) {
                        response = std::make_unique<file_response>(index_path, EResponseCode::EST_OK);
                        break;
                    }
                }
            }

            // Generate 404 Response
            if (!response) {
                response = std::make_unique<json_response>(boost::json::object{{"message", "null"}}, EResponseCode::EST_NOT_FOUND);
            }

            // Matching Response
            if (response) {
                for (auto&middleware: middlewares_after_) {
                    co_await middleware->after_handle(*request, *response);
                }
                std::unique_ptr<core::http_iodata> response_data = response->serialize();
                co_await write_response_(socket, response_data);
            }
        }
    }

    boost::asio::awaitable<http::http_header> http_server::receive_header_(
        boost::asio::ip::tcp::socket&socket, boost::asio::streambuf&buffer) {
        http_header header{};
        std::string_view bytes_view;
        do {
            const auto bytes_transferred = co_await socket.async_read_some(
                buffer.prepare(1024 * 10), boost::asio::use_awaitable);
            buffer.commit(bytes_transferred);
            bytes_view = std::string_view(boost::asio::buffer_cast<const char *>(buffer.data()), buffer.size());
        }
        while (buffer.size() < 1024 * 10 && !bytes_view.contains("\r\n\r\n"));

        if (!bytes_view.contains("\r\n\r\n"))
            throw protocol_exception("Header Size Exceed, Shutting Down!");

        bytes_view = std::string_view(bytes_view.data(), bytes_view.find("\r\n\r\n") + 4);
        if (!parser::parse_http_header(bytes_view, header))
            throw protocol_exception("Header Parse Failed, Shutting Down!");
        buffer.consume(bytes_view.size());
        co_return header;
    }

    boost::asio::awaitable<std::unique_ptr<std::iostream>> http_server::receive_body_(
        boost::asio::ip::tcp::socket&socket, boost::asio::streambuf&buffer, http_header&header) {
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
            ret->write(boost::asio::buffer_cast<const char*>(buffer.data()), total_transferred);
            buffer.consume(total_transferred);
        }
        while (total_transferred < content_length) {
            const auto bytes_wanna_read = std::min<uint32_t>(content_length - total_transferred, 1024 * 10);
            const auto transferred = co_await socket.async_read_some(buffer.prepare(bytes_wanna_read), boost::asio::use_awaitable);
            buffer.commit(transferred);
            total_transferred += transferred;
            ret->write(boost::asio::buffer_cast<const char *>(buffer.data()), transferred);
            buffer.consume(transferred);
        }

        ret->flush();
        co_return ret;
    }

    boost::asio::awaitable<void> http_server::write_response_(boost::asio::ip::tcp::socket&socket, const std::unique_ptr<core::http_iodata>&response) {
        unsigned char buffer[1024 * 256] = {};
        while (!response->eof()) {
            const auto bytes_read = response->read(buffer, 1024 * 256);
            auto bytes_transferred = co_await socket.async_write_some(boost::asio::const_buffer(buffer, bytes_read), boost::asio::use_awaitable);
        }
        co_return;
    }
} // http::obelisk
