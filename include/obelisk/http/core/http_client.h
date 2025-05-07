//
// Created by Hackman.Lo on 2024/9/23.
//

#ifndef OBELISK_HTTP_CLIENT_H
#define OBELISK_HTTP_CLIENT_H
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/cobalt/task.hpp>
#include <obelisk/core/coroutine/task.h>
#include "raw.h"

#include "http_iodata.h"
#include "io.h"

namespace obelisk::http{
    class http_response;
    namespace core {
        using namespace boost::asio;
        class http_client {

        public:
            explicit http_client() {}

            task<std::shared_ptr<http_response>> send_request(const std::string &uri, const std::string& method, std::unordered_map<std::string, std::string> headers, std::unique_ptr<http_iodata> body);

            virtual ~http_client() = default;
        protected:
            virtual void before_send_() {}
            template<typename StreamType>
            static awaitable<void> perform_request_(StreamType& stream,const std::unique_ptr<http_iodata>& data) {
                co_await io::write_data_(stream, data);
                streambuf buff;
                auto response_header_raw = co_await io::receive_header_(stream, buff);
                auto response_body = co_await io::receive_body_(stream, buff, response_header_raw);
                response_body->seekg(0, std::ios::beg);
                std::string resp_str;
                resp_str.reserve(1024*1024*2);
                response_body->read(resp_str.data(), 1024*1024*2);
                co_return;
            }
            std::unique_ptr<http_iodata> make_iodata_(raw::http_header_raw& raw_header, std::unique_ptr<http_iodata> body);
            static std::unique_ptr<boost::asio::ssl::context> _ssl_context;
        };
    }

} // core::http::obelisk

#endif //OBELISK_HTTP_CLIENT_H
