//
// Created by hackman on 5/7/25.
//

#ifndef OBELISK_HTTP_CORE_IO_H
#define OBELISK_HTTP_CORE_IO_H
#include <iostream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <obelisk/http/exception/protocol_exception.h>
#include <obelisk/http/parser/http_parser_v3.h>

#include "raw.h"
namespace obelisk::http::core
{
    using namespace boost::asio;
    class io {
    public:
        template <typename StreamType>
        static awaitable<raw::http_header_raw> receive_header_(StreamType& stream, streambuf &buffer)
        {
            raw::http_header_raw header{};
            std::string_view bytes_view;
            do {
                const auto [ec, bytes_transferred] = co_await stream.async_read_some(buffer.prepare(1024 * 10), as_tuple(use_awaitable));
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
        template <typename StreamType>
        static awaitable<std::unique_ptr<std::iostream>> receive_body_(StreamType &socket, streambuf& buffer, raw::http_header_raw& header)
        {
            if (header.headers_.contains("Transfer-Encoding") && boost::algorithm::iequals(header.headers_["Transfer-Encoding"], "chunked")) {
                co_return co_await receive_chunked_body_(socket, buffer);
            }
            if (!header.headers_.contains("Connection") || boost::algorithm::iequals(header.headers_["Connection"], "close")) {
                co_return co_await receive_until_dead_(socket, buffer);
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
                const auto [ec,transferred] = co_await async_read(socket, buffer.prepare(bytes_wanna_read), as_tuple(use_token));
                buffer.commit(transferred);
                total_transferred += transferred;
                ret->write(static_cast<const char *>(buffer.data().data()), transferred);
                buffer.consume(transferred);
                if (ec) break;
            }

            ret->flush();
            co_return ret;
        }

        template <typename StreamType>
        static awaitable<void> write_data_(StreamType& socket, const std::unique_ptr<http_iodata>& response)
        {
            unsigned char buffer[1024 * 256] = {};
            while (!response->eof()) {
                const auto bytes_read = response->read(buffer, 1024 * 256);
                std::uint64_t bytes_transferred = 0;
                while (bytes_transferred< bytes_read){
                    auto [ec, transferred] =co_await socket.async_write_some(boost::asio::const_buffer(&buffer[bytes_transferred], bytes_read - bytes_transferred), as_tuple(use_token));
                    if (ec) throw std::runtime_error(ec.message());
                    bytes_transferred += transferred;
                }
            }
            co_return;
        }

    private:
        template<typename StreamType>
        static awaitable<std::unique_ptr<std::iostream>> receive_chunked_body_(StreamType& stream, streambuf& buffer)
        {
            std::cout << "Chunked is Not Implemented" << std::endl;
            co_return nullptr;
        }

        template<typename StreamType>
        static awaitable<std::unique_ptr<std::iostream>> receive_until_dead_(StreamType& stream, streambuf& buffer)
        {
            std::filesystem::create_directories("./temp");
            auto ret = std::make_unique<http_temp_fstream>("./temp/" + sahara::utils::uuid::generate());
            while (true) {
                const auto [ec,transferred] = co_await async_read(stream, buffer.prepare(1024 * 10), as_tuple(use_token));
                buffer.commit(transferred);
                ret->write(static_cast<const char *>(buffer.data().data()), transferred);
                buffer.consume(transferred);
                if (ec) break;
            }
            ret->flush();
            co_return ret;
        }
    };

}

#endif //OBELISK_HTTP_CORE_IO_H
