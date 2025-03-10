#ifndef MEMBER_CONTROLLER_H
#define MEMBER_CONTROLLER_H

#include "obelisk/http/response/json_response.h"
#include <obelisk/core/coroutine/async_scoped_lock.h>
#include <obelisk/core/coroutine/task.h>

#include "controller_base.h"
#include "obelisk/http/core/http_request.h"

using namespace obelisk::http::validator;
class member_controller: public controller_base {
public:
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> view(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> create(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> update(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> freeze(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> sign_card(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> soft_delete(obelisk::http::http_request_wrapper&request);

    // OLD
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> findByPreciseData(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> getCardOwner(obelisk::http::http_request_wrapper&request);
    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> entrance(obelisk::http::http_request_wrapper&request);
};

#endif //USER_CONTROLLER_H
