//
// Created by Hackman.Lo on 2024/9/23.
//

#ifndef OBELISK_HTTP_CLIENT_H
#define OBELISK_HTTP_CLIENT_H
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl.hpp>

namespace obelisk::http{
    class http_response;
    namespace core {

        class http_client {

        public:
            explicit http_client(boost::asio::io_context &ioctx) {
                if (!_ssl_context) {
                    _ssl_context = std::make_shared<boost::asio::ssl::context>(
                            boost::asio::ssl::context::tlsv12_client);
                    _load_system_certificates();
                }
            }

            boost::asio::awaitable<std::shared_ptr<http_response>>
            send_request(const std::string &uri, std::unordered_map<std::string, std::string> headers,
                         std::shared_ptr<std::istream> body);

        protected:
            static std::shared_ptr<boost::asio::ssl::context> _ssl_context;

            static bool _load_system_certificates();

            static bool _load_system_certificates(boost::system::error_code &error);
        };
    }

} // core::http::obelisk

#endif //OBELISK_HTTP_CLIENT_H
