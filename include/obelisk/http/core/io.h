//
// Created by hackman on 5/7/25.
//

#ifndef OBELISK_HTTP_CORE_IO_H
#define OBELISK_HTTP_CORE_IO_H
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <obelisk/http/exception/protocol_exception.h>
#include <obelisk/http/parser/http_parser_v3.h>
#include <boost/iostreams/filtering_stream.hpp>
#include "raw.h"
#include <boost/iostreams/filter/gzip.hpp>
namespace obelisk::http::core
{
    class io {
    public:
        template <typename StreamType>
        static boost::asio::awaitable<raw::http_header_raw> receive_header_(StreamType& stream, boost::asio::streambuf &buffer)
        {
            raw::http_header_raw header{};
            std::string_view bytes_view;
            do {
                const auto [ec, bytes_transferred] = co_await stream.async_read_some(buffer.prepare(1024 * 10), boost::asio::as_tuple(boost::asio::use_awaitable));
                buffer.commit(bytes_transferred);
                bytes_view = std::string_view(static_cast<const char *>(buffer.data().data()), buffer.size());
                if (ec) break;
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
        static boost::asio::awaitable<std::unique_ptr<std::iostream>> receive_body_(StreamType &socket, boost::asio::streambuf& buffer, raw::http_header_raw& header)
        {
            if (header.headers_.contains("Transfer-Encoding") && boost::algorithm::iequals(header.headers_["Transfer-Encoding"], "chunked")) {
                co_return co_await receive_chunked_body_(socket, buffer, header.headers_["Content-Encoding"]);
            }
            if (!header.headers_.contains("Connection") || boost::algorithm::iequals(header.headers_["Connection"], "close")) {
                co_return co_await receive_until_dead_(socket, buffer);
            }
            if (!header.headers_.contains("Content-Length")) co_return nullptr;
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
                const auto [ec,transferred] = co_await async_read(socket, buffer.prepare(bytes_wanna_read), boost::asio::as_tuple(use_token));
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
        static boost::asio::awaitable<void> write_data_(StreamType& socket, const std::unique_ptr<base_iodata>& response)
        {
            unsigned char buffer[1024 * 256] = {};
            while (!response->eof()) {
                const auto bytes_read = response->read(buffer, 1024 * 256);
                std::uint64_t bytes_transferred = 0;
                while (bytes_transferred< bytes_read){
                    auto [ec, transferred] =co_await socket.async_write_some(boost::asio::const_buffer(&buffer[bytes_transferred], bytes_read - bytes_transferred), boost::asio::as_tuple(use_token));
                    if (ec) throw std::runtime_error(ec.message());
                    bytes_transferred += transferred;
                }
            }
            co_return;
        }

    private:
        template<typename StreamType>
        static boost::asio::awaitable<std::unique_ptr<std::iostream>> receive_chunked_body_(StreamType& stream, boost::asio::streambuf& buffer, const std::string& encoding)
        {
            auto stream_ptr = std::make_unique<http_temp_fstream>("./temp/" + sahara::utils::uuid::generate());
            boost::iostreams::filtering_ostream out;
            if (encoding == "gzip")
                out.push(boost::iostreams::gzip_decompressor());
            out.push(*stream_ptr);

            do
            {
                auto size_pos = co_await receive_until_(stream, buffer, "\r\n");
                std::string_view size_view(static_cast<const char *>(buffer.data().data()), size_pos);
                auto content_length = std::stoull(std::string(size_view),nullptr , 16);
                buffer.consume(size_pos + 2);
                if (content_length == 0) break;

                std::uint64_t bytes_should_transferred = content_length;
                do
                {
                    std::string_view v1(static_cast<const char *>(buffer.data().data()), buffer.data().size());
                    auto received = co_await receive_until_(stream, buffer, std::min<uint64_t>(bytes_should_transferred, 1024 * 10));
                    bytes_should_transferred -= received;

                    out.write(static_cast<const char *>(buffer.data().data()), received);
                    buffer.consume(received);
                }while (bytes_should_transferred>0);
                co_await receive_until_(stream, buffer, 2);
                buffer.consume(2);
            }while (true);
            co_await receive_until_(stream, buffer, 2);
            buffer.consume(2);
            co_return stream_ptr;
        }

        template <typename StreamType>
        static boost::asio::awaitable<std::uint64_t> receive_until_(StreamType& stream, boost::asio::streambuf& buffer, const std::string& delimiter)
        {
            std::string_view bytes_view(static_cast<const char *>(buffer.data().data()), buffer.size());
            do {
                if (auto pos = bytes_view.find(delimiter); pos != std::string_view::npos)
                    co_return pos;
                const auto [ec, bytes_transferred] = co_await stream.async_read_some(buffer.prepare(1024 * 10), boost::asio::as_tuple(boost::asio::use_awaitable));
                buffer.commit(bytes_transferred);
                bytes_view = std::string_view(static_cast<const char *>(buffer.data().data()), buffer.size());
                if (ec) break;
            }while (buffer.size() < 1024 * 10 && !bytes_view.contains(delimiter));

            auto pos = bytes_view.find(delimiter);
            if (pos == std::string_view::npos)
                throw protocol_exception("Protocol Error, Shutting Down!");
            co_return pos;
        }

        template <typename StreamType>
        static boost::asio::awaitable<std::uint64_t> receive_until_(StreamType& stream, boost::asio::streambuf& buffer, std::uint64_t size)
        {
            if (buffer.size()>=size) co_return std::min(buffer.size(), size);
            do {
                const auto [ec, bytes_transferred] = co_await stream.async_read_some(buffer.prepare(1024 * 10), boost::asio::as_tuple(boost::asio::use_awaitable));
                buffer.commit(bytes_transferred);
                if (ec) throw protocol_exception("Protocol Error, Shutting Down!");
            }while (buffer.size() < std::min<std::uint64_t>(size  , 1024 * 10));
            co_return std::min(buffer.size(), size);
        }

        template<typename StreamType>
        static boost::asio::awaitable<std::unique_ptr<std::iostream>> receive_until_dead_(StreamType& stream, boost::asio::streambuf& buffer)
        {
            std::filesystem::create_directories("./temp");
            auto ret = std::make_unique<http_temp_fstream>("./temp/" + sahara::utils::uuid::generate());
            while (true) {
                const auto [ec,transferred] = co_await async_read(stream, buffer.prepare(1024 * 10), boost::asio::as_tuple(use_token));
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
