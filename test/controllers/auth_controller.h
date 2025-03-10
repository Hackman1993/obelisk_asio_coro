#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "obelisk/http/response/json_response.h"
#include <obelisk/core/coroutine/async_scoped_lock.h>

#include "controller_base.h"
#include "obelisk/http/core/http_request.h"

using namespace obelisk::http::validator;
class auth_controller: public controller_base {
public:
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> login(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> login1(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> logout(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> check_auth(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> get_permissions(obelisk::http::http_request_wrapper&request);
};

#endif //AUTH_CONTROLLER_H
