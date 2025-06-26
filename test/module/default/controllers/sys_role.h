//
// Created by hackman on 5/21/25.
//

#ifndef SYS_ROLE_H
#define SYS_ROLE_H
#include <obelisk/database/db.h>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>

#include "controllers/controller.h"
namespace module::default_::controllers
{
    using namespace boost::asio;
    using namespace obelisk::http;
    class sys_role : public ::controllers::controller{
    public:
        static awaitable<std::unique_ptr<http_response>> backend_view(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_create(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_update(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_delete(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_assignable_permission(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_assign_permission(http_request_wrapper&request);
    };

}

#endif //SYS_ROLE_H
