/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_server.cpp
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/

#include "http/http_server.h"
#include "http/response/file_response.h"
#include "http/exception/http_exception.h"
#include "http/response/json_response.h"
#include "http/core/http_request.h"
#include "http/middleware/middleware.h"
#include <boost/json.hpp>
#include "http/parser/http_parser_v2.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "http/exception/protocol_exception.h"
#include "http/router/route_item.h"

namespace obelisk::http {
    http_server::http_server(boost::asio::io_context&ctx, const std::string&webroot) : acceptor_(ctx) {
        //            if(!(std::filesystem::exists(webroot_) && std::filesystem::is_directory(webroot_)))
        //                throw std::logic_error("Root Dirctory Not Exists");
        //            acceptor_.on_accepted([&](const std::shared_ptr<http_connection>& accepted){
        //                accepted->on_request_received(std::bind(&http_server::on_request_, this, std::placeholders::_1));
        //            });
    }

    //        std::shared_ptr<route_item>& http_server::route(const std::string& route, std::function<std::shared_ptr<http_response> (std::shared_ptr<http_request>&)> handler){
    //            return routes_.emplace_back(std::make_shared<route_item>(route, handler));
    //        }

    //        std::shared_ptr<http_response> http_server::on_request_(std::shared_ptr<http_request> &request) {
    //            std::shared_ptr<http_response> resp = nullptr;
    //            {
    //                for(const auto& ptr : middlewares_){
    //                    auto derived_ptr = std::dynamic_pointer_cast<before_middleware>(ptr);
    //                    if(derived_ptr)
    //                        resp = derived_ptr->pre_handle(request);
    //                    if(resp)
    //                        break;
    //                }
    //
    //            }
    //            if(!resp){
    //                for(const auto&ptr : routes_){
    //                    std::unordered_map<std::string,std::string> route_params;
    //                    if(!ptr->match(std::string(request->path()), route_params))
    //                        continue;
    //                    if(!ptr->method_allowed(request->method()))
    //                        THROW(http_exception, "Method Not Allowed!", "Obelisk", EResponseCode::EST_METHOD_NOT_ALLOWED);
    //                    request->route_params_ = route_params;
    //                    resp = ptr->handle(request);
    //                    if(resp) break;
    //                }
    //            }
    //            if(!resp){
    //                std::filesystem::path path(webroot_);
    //                path.append(std::string(".").append(request->path()));
    //                if(std::filesystem::exists(path)){
    //                    if(!std::filesystem::is_directory(path)){
    //                        resp = std::make_shared<file_response>(path.string(), EResponseCode::EST_OK);
    //                    }else{
    //                        for(const auto& index: index_files_){
    //                            std::filesystem::path index_path(path);
    //                            index_path.append(index);
    //                            if(std::filesystem::exists(index_path) && !std::filesystem::is_directory(index_path))
    //                                resp = std::make_shared<file_response>(index_path.string(), EResponseCode::EST_OK);
    //                        }
    //                    }
    //                }
    //            }
    //
    //            if(!resp)
    //                resp = std::make_shared<json_response>(boost::json::object({{"code",    404}, {"message", "Not Found"}, {"data_",    boost::json::value()}}), EResponseCode::EST_NOT_FOUND);
    //
    //            for(const auto& ptr : middlewares_){
    //                auto derived_ptr = std::dynamic_pointer_cast<after_middleware>(ptr);
    //                if(derived_ptr) derived_ptr->after_handle(request, resp);
    //            }
    //
    //            resp->add_header("Host", request->header("Host"));
    //            resp->add_header("Cache-Control", "no-cache, private");
    //            resp->add_header("Vary", "Origin");
    //            resp->add_header("Content-Length", std::to_string(resp->content_length()));
    //            return resp;
    //        }

    void http_server::listen(const std::string&address, unsigned short port) {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(address), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.bind(endpoint);
        acceptor_.listen();
        co_spawn(acceptor_.get_executor(), listen_(), boost::asio::detached);
    }

    boost::asio::awaitable<void> http_server::listen_() {
        while (true) {
            auto socket = co_await acceptor_.async_accept(boost::asio::use_awaitable);
            co_spawn(this->acceptor_.get_executor(), handle_(std::move(socket)), boost::asio::detached);
        }
    }

    boost::asio::awaitable<void> http_server::handle_(boost::asio::ip::tcp::socket socket) {
        boost::asio::streambuf buffer;
        while (true) {
            uint32_t request_bytes = 0;
            std::shared_ptr<http_response> response;
            try {
                auto header = co_await receive_header_(socket, buffer);;
                std::unique_ptr<std::iostream> body = co_await receive_body_(socket, buffer, header);
                http_request_wrapper request(header, std::move(body));
                for (auto &before_middleware: middlewares_before_) {
                    response = before_middleware->pre_handle(request);
                    if(response) break;
                }

                if(!response) {
                    for(const auto&ptr : routes_){
                        std::unordered_map<std::string,std::string> route_params;
                        if(!ptr->match(std::string(request.path()), route_params))
                            continue;
                        if(!ptr->method_allowed(request.method()))
                            THROW(http_exception, "Method Not Allowed!", "Obelisk", EResponseCode::EST_METHOD_NOT_ALLOWED);
                        response = ptr->handle(request);
                        if(response) break;
                    }
                }
            }
            catch (const boost::system::system_error&e) {
            }
        }
    }

    boost::asio::awaitable<http::http_header> http_server::receive_header_(boost::asio::ip::tcp::socket &socket, boost::asio::streambuf&buffer) {
        http_header header{};
        std::string_view bytes_view;
        do {
            const auto bytes_transferred = co_await socket.async_read_some(buffer.prepare(1024 * 10), boost::asio::use_awaitable);
            buffer.commit(bytes_transferred);
            bytes_view = std::string_view(boost::asio::buffer_cast<const char *>(buffer.data()), buffer.size());
        }
        while (buffer.size() < 1024 * 10 && bytes_view.contains("\r\n\r\n"));

        if (!bytes_view.contains("\r\n\r\n"))
            throw protocol_exception("Header Size Exceed, Shutting Down!");

        bytes_view = std::string_view(bytes_view.data(), bytes_view.find("\r\n\r\n") + 4);
        if (!parser::parse_http_header(bytes_view, header))
            throw protocol_exception("Header Parse Failed, Shutting Down!");
        buffer.consume(bytes_view.size());
        co_return header;
    }

    boost::asio::awaitable<std::unique_ptr<std::iostream>> http_server::receive_body_(boost::asio::ip::tcp::socket &socket, boost::asio::streambuf& buffer, http_header& header) {
        if(!header.headers_.contains("Content-Length")) {
            co_return nullptr;
        }
        const auto content_length = std::stoul(header.headers_["Content-Length"]);
        if(content_length == 0) co_return nullptr;

        std::unique_ptr<std::iostream> ret;
        if(content_length < 1024*1024*1)
            ret = std::make_unique<std::stringstream>();
        else
            ret = std::make_unique<http_temp_fstream>("./" + sahara::utils::uuid::generate());

        uint32_t total_transferred = 0;
        do {
            const auto bytes_wanna_read = std::min<uint32_t>(content_length - total_transferred, 1024* 10);
            const auto transferred = co_await socket.async_read_some(buffer.prepare(bytes_wanna_read), boost::asio::use_awaitable);
            buffer.commit(transferred);
            total_transferred += transferred;
            ret->write(boost::asio::buffer_cast<const char *>(buffer.data()), transferred);
        } while (total_transferred < content_length);

        ret->flush();
        co_return ret;
    }
} // http::obelisk
