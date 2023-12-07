/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_connection.cpp
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/
#include <iostream>
#include <sahara/utils/uuid.h>
#include "http/http_connection.h"
#include "http/parser/http_parser_v2.h"
#include "http/core/http_request.h"
#include "http/core/http_response.h"
#include "http/exception/protocol_exception.h"

namespace obelisk::http {
    http_connection::http_connection(boost::asio::ip::tcp::socket &socket) : socket_base(socket) {
        request_ = std::make_shared<http_block_data>();
    }

    void http_connection::e_connected() {

    }

    void http_connection::e_data_sent(std::size_t bytes_transferred) {
        write_outgoing_request();
    }

    bool http_connection::e_data_received(std::size_t bytes_transferred) {
        bool finished;
        do {
            finished = expecting_body_ ? handle_body_() : handle_header_();
            if (finished) {
                auto request = std::make_shared<http_request>(request_);
                e_request_received(request);
                request_ = std::make_shared<http_block_data>();
            }
        } while (finished && instream_.size() > 0);
        return true;
    }

    bool http_connection::handle_header_() {
        // std::string_view received_data(boost::asio::buffer_cast<const char *>(instream_.data()), instream_.size());
        // bool contains_header_eof = received_data.contains("\r\n\r\n");
        // if (instream_.size() > 10 * 1024 && !contains_header_eof)
        //     THROW(protocol_exception, "Header Size Exceed, Shutting Down!", "Obelisk");
        // if (!contains_header_eof) return false;
        // received_data = std::string_view(received_data.data(), received_data.find("\r\n\r\n") + 4);
        // auto parse_result = parser::parse_http_header(received_data, *request_);
        // if (!parse_result || !request_->header_.meta_.is_req_)
        //     THROW(protocol_exception, "Invalid Package Header, Shutting Down!", "Obelisk");
        // instream_.consume(received_data.size());
        if (request_->header_.headers_.contains("Content-Length")) {
            request_->content_length_ = std::stoull(request_->header_.headers_["Content-Length"]);
            body_reamains_ = request_->content_length_;
            request_->prepare_content(request_->content_length_);
        }
        // if (request_->content_length_ > 0) {
        //     expecting_body_ = true;
        //
        //     if (instream_.size() > 0)
        //         return handle_body_();
        //
        // } else return true;
        //
         return false;
    }

    bool http_connection::handle_body_() {
        std::string_view received_data(boost::asio::buffer_cast<const char *>(instream_.data()), instream_.size());
        // Calculate data_ should be written, prevent buffer contains some part of content and a new request header
        std::uint32_t bytes_write = std::min<std::uint64_t>(received_data.size(), body_reamains_);
        if (bytes_write > 0) {
            request_->content_->write(received_data.data(), bytes_write);
            instream_.consume(bytes_write);
            body_reamains_ -= bytes_write;
        }
        // if request has been fully received, flush data_
        if (body_reamains_ == 0) {
            expecting_body_ = false;
            request_->content_->flush();
            return true;
        }
        return false;
    }

    void http_connection::e_request_received(std::shared_ptr<http_request>& request) {
        if(f_requested_){
            auto ret = f_requested_(request);
            if(ret) write_response(ret);
        }
        std::cout << "Request Received" << std::endl;
    }

    void http_connection::write_response(const std::shared_ptr<http_response> &resp) {
        out_requests_.push(resp->serialize());
        write_outgoing_request();
    }

    void http_connection::write_outgoing_request() {
        if (write_pending_ || out_requests_.empty() || outstream_.in_avail() >= 1 * 1024 * 1024) return;
        auto req = out_requests_.front();
        auto buffer_len = 1*1024*1024 - outstream_.size();
        auto buffer = outstream_.prepare(buffer_len);

        auto bytes_read = req->read(static_cast<unsigned char *>(buffer.data()), buffer_len);
        outstream_.commit(bytes_read);
        req->consume(bytes_read);
        if(bytes_read < buffer_len)
            out_requests_.pop();
        send();
    }

    void http_connection::on_request_received(const std::function<std::shared_ptr<http_response>(std::shared_ptr<http_request> &)> &callback) {
        f_requested_ = callback;
    }

    boost::asio::awaitable<void> http_connection::handle_co_() {
        return boost::asio::awaitable<void>();
    }
} // obelisk::http