#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "obelisk/http/response/json_response.h"
#include <obelisk/core/coroutine/async_scoped_lock.h>

#include "controller_base.h"
#include "obelisk/http/core/http_request.h"
#include "obelisk/core/coroutine/task.h"

using namespace obelisk::http::validator;
class user_controller: public controller_base {
public:
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> view(obelisk::http::http_request_wrapper&request);
};

#endif //USER_CONTROLLER_H
