#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "obelisk/http/response/json_response.h"
#include <obelisk/core/coroutine/async_scoped_lock.h>

#include "controller_base.h"
#include "obelisk/http/core/http_request.h"

namespace controller
{
    using namespace obelisk::http;
    class auth_controller: public controller_base {
    public:
        static obelisk::task<std::unique_ptr<http_response>> login(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> login1(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> logout(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> check_auth(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> get_permissions(http_request_wrapper&request);
    };
}

#endif //AUTH_CONTROLLER_H
