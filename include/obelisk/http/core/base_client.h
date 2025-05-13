//
// Created by Hackman.Lo on 2024/9/23.
//

#ifndef OBELISK_HTTP_CLIENT_H
#define OBELISK_HTTP_CLIENT_H
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl.hpp>
#include <obelisk/core/coroutine/task.h>
#include "raw.h"
#include "io_data.h"
#include "http_iodata_stream_wrapper.h"
#include "http_response.h"
#include "io.h"
#include <obelisk/http/client/signer/base_signer.h>
namespace obelisk::http{
    class http_response;
    namespace core {
        using namespace boost::asio;
        class base_client {

        public:
            explicit base_client() = default;
            explicit base_client(std::unique_ptr<client::signer::base_signer> signer):signer_(std::move(signer)) {}
            virtual awaitable<std::unique_ptr<http_response>> send_request(const std::string &uri, const std::string& method, std::unordered_map<std::string, std::string> headers, std::unique_ptr<base_iodata> body);
            virtual ~base_client() = default;
        protected:
            template<typename StreamType>
            static awaitable<std::unique_ptr<http_response>> perform_request_(StreamType& stream,const std::unique_ptr<base_iodata>& data, const std::string& method) {
                co_await io::write_data_(stream, data);
                streambuf buff;
                auto response_header_raw = co_await io::receive_header_(stream, buff);
                std::unique_ptr<std::iostream> response_body;
                if (method!="HEAD")
                    response_body = co_await io::receive_body_(stream, buff, response_header_raw);
                if (response_body)
                    response_body->seekg(std::ios::beg);
                co_return std::make_unique<http_response>(response_header_raw, response_body?std::make_unique<http_data_istream_wrapper>(std::move(response_body)):nullptr);
            }
            static std::unique_ptr<base_iodata> make_iodata_(raw::http_request_raw& request);
            static std::unique_ptr<ssl::context> _ssl_context;
            std::unique_ptr<client::signer::base_signer> signer_;
        };
    }

} // core::http::obelisk

#endif //OBELISK_HTTP_CLIENT_H
