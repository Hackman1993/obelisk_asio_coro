//
// Created by hackman on 5/15/25.
//

#ifndef SYS_ADMIN_CONTROLLER_H
#define SYS_ADMIN_CONTROLLER_H
#include <obelisk/database/db.h>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>

#include "controller.h"

namespace module::default_::controllers
{
    class sys_admin_controller : public controller{
    public:
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_view(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_create(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_update(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_delete(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_assignable_role(obelisk::http::http_request_wrapper&request);
    };
}

#endif //SYS_ADMIN_CONTROLLER_H
