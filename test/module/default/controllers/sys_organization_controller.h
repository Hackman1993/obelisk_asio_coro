//
// Created by hackman on 5/15/25.
//

#ifndef SYS_ORGANIZATION_CONTROLLER_H
#define SYS_ORGANIZATION_CONTROLLER_H


#include "controllers/controller.h"

namespace module::default_::controllers
{
    using namespace boost::asio;
    using namespace obelisk::http;
    class sys_organization_controller : ::controllers::controller{
    public:
        static awaitable<std::unique_ptr<http_response>> backend_view(http_request_wrapper&request);

        static awaitable<std::unique_ptr<http_response>> backend_create(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_update(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_delete(http_request_wrapper&request);
    };
}

#endif //SYS_ORGANIZATION_CONTROLLER_H
