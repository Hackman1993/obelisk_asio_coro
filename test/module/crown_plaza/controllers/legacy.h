//
// Created by hackman on 6/15/25.
//

#ifndef LEGACY_H
#define LEGACY_H
#include "controllers/controller.h"

namespace module::crown_plaza::controllers
{
    class legacy: public ::controllers::controller {
    public:
        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> get_terminal_token(obelisk::http::http_request_wrapper &request);
        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> test(obelisk::http::http_request_wrapper &request);
    };
}

#endif //LEGACY_H
