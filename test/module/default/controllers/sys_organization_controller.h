//
// Created by hackman on 5/15/25.
//

#ifndef SYS_ORGANIZATION_CONTROLLER_H
#define SYS_ORGANIZATION_CONTROLLER_H


#include "controller.h"

namespace module::default_::controllers
{
    class sys_organization_controller : controller{
    public:
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_view(obelisk::http::http_request_wrapper&request);

        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_create(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_update(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_delete(obelisk::http::http_request_wrapper&request);
    };
}

#endif //SYS_ORGANIZATION_CONTROLLER_H
