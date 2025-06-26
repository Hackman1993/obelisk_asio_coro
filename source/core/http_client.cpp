//
// Created by Hackman.Lo on 2024/9/23.
//

#include "obelisk/http/core/base_client.h"
#include "obelisk/http/core/http_response.h"
#include <iostream>
#include <sahara/log/log.h>
#ifdef _WIN32
#include <wincrypt.h>
#endif
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <sahara/exception/exception_base.h>

#include "obelisk/http/core/io.h"
#include "obelisk/http/core/http_iodata_stream_wrapper.h"
namespace obelisk::http::core {
    using namespace  boost::asio;
    std::unique_ptr<ssl::context> base_client::_ssl_context = nullptr;

    awaitable<std::unique_ptr<http_response>>base_client::send_request(const std::string &uri, const std::string& method, std::unordered_map<std::string, std::string> headers,std::unique_ptr<base_iodata> body) {
        auto executor = co_await this_coro::executor;
        boost::regex url_regex(R"(^(?<protocol>http|https):\/\/(?<domain>[a-zA-Z0-9.-]+)(?::(?<port>\d+))?(?<path>/.*)$)", boost::regex::icase | boost::regex::no_char_classes);

        boost::smatch match;
        if (!regex_match(uri, match, url_regex))
            THROW(sahara::exception::exception_base, "Invalid URL!", "HttpClient");
        auto use_ssl = boost::algorithm::iequals(match["protocol"].str(), "https");
        auto port = match["port"].matched? match["port"].str():(use_ssl?"443":"80");
        raw::http_header_raw raw;
        raw.meta_.p1_ = boost::to_upper_copy(method);
        raw.meta_.p2_ = match["path"].matched? match["path"].str():"/";
        raw.meta_.p3_ = "HTTP/1.1";
        raw.headers_.emplace("Host", std::format("{}{}", match["domain"].str(), match["port"].matched? ":" + match["port"].str(): ""));
        //raw.headers_.emplace("Accept", "*/*");
        //raw.headers_.emplace("Accept-Encoding", "gzip,deflate,br");
        for (auto& [fst, snd]: headers)
            raw.headers_.emplace(fst, snd);

        auto raw_request = raw::http_request_raw{raw, std::move(body)};
        if (signer_)
            signer_->sign_request(raw_request);

        ip::tcp::resolver resolver(executor);
        auto [ec, endpoints] = co_await resolver.async_resolve(ip::tcp::v4(), match["domain"].str(), port, as_tuple(use_awaitable));
        if (ec)
            throw std::logic_error(ec.message());

        ip::tcp::socket socket(executor);
        if (auto [connect_ec, ep] = co_await async_connect(socket, endpoints, as_tuple(use_awaitable)); connect_ec)
            throw std::logic_error(connect_ec.message());

        auto data  = make_iodata_(raw_request);
        if (use_ssl)
        {
            if (!_ssl_context)
            {
                _ssl_context = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
                _ssl_context->set_verify_mode(ssl::verify_peer);
                _ssl_context->set_default_verify_paths();
                _ssl_context->add_verify_path("/etc/ssl/certs/");
            }
            ssl::stream<ip::tcp::socket> stream(std::move(socket), *_ssl_context);

            if (auto [handshake_ec] = co_await stream.async_handshake(ssl::stream_base::client, as_tuple(use_awaitable)); handshake_ec)
            {
                std::cout << handshake_ec.message() << std::endl;
                throw std::logic_error(handshake_ec.message());
            }
            co_return co_await perform_request_(stream, data, method);
        }
        co_return co_await perform_request_(socket, data, method);
    }

    std::unique_ptr<base_iodata> base_client::make_iodata_(raw::http_request_raw& request)
    {
        auto &raw_header = request.header_;
        auto result =  std::make_unique<multi_stream_iodata>();
        auto ssp = std::make_unique<std::stringstream>();
        (*ssp) << std::format( "{} {} HTTP/1.1\r\n", raw_header.meta_.p1_, raw_header.meta_.p2_);
        for(auto & [fst, snd]: raw_header.headers_)
            (*ssp) << std::format("{}: {}\r\n", fst, snd);
        (*ssp) << "\r\n";
        result->append(std::make_unique<http_data_istream_wrapper>(std::move(ssp)));
        if(request.body_)
            result->append(std::move(request.body_));
        return result;

    }
} // core
