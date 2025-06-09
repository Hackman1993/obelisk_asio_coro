//
// Created by hackman on 5/30/25.
//

#ifndef MEMBER_CONTROLLER_H
#define MEMBER_CONTROLLER_H
#include <boost/asio/awaitable.hpp>

#include "controllers/controller.h"
#include <obelisk/http/core/http_response.h>
#include <obelisk/http/core/http_request.h>

namespace module::crown_plaza::controllers
{
    using namespace boost::asio;
    using namespace obelisk::http;
    class member_controller : protected ::controllers::controller{
    public:
        static awaitable<std::unique_ptr<http_response>> backend_view(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_create(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_update(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_delete(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_sign_card(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_freeze_card(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> entrance(http_request_wrapper&request);
    };

}

#endif //MEMBER_CONTROLLER_H
