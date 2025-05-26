//
// Created by hackman on 5/21/25.
//

#ifndef SYS_ROLE_H
#define SYS_ROLE_H
#include <obelisk/database/db.h>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>

#include "controller.h"
namespace module::default_::controllers
{

    class sys_role : public controller{
    public:
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_view(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_create(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_update(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_delete(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_assignable_permission(obelisk::http::http_request_wrapper&request);
        static awaitable<std::unique_ptr<obelisk::http::http_response>> backend_assign_permission(obelisk::http::http_request_wrapper&request);
    };

}

#endif //SYS_ROLE_H
