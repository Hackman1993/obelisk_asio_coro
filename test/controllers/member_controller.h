#ifndef MEMBER_CONTROLLER_H
#define MEMBER_CONTROLLER_H

#include "obelisk/http/response/json_response.h"
#include <obelisk/core/coroutine/async_scoped_lock.h>
#include <obelisk/core/coroutine/task.h>

#include "controller_base.h"
#include "obelisk/http/core/http_request.h"

namespace controller
{
    using namespace obelisk::http;
    class member_controller: public controller_base {
    public:
        static obelisk::task<std::unique_ptr<http_response>> view(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> create(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> update(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> freeze(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> sign_card(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> soft_delete(http_request_wrapper&request);

        // OLD
        static obelisk::task<std::unique_ptr<http_response>> findByPreciseData(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> getCardOwner(http_request_wrapper&request);
        static obelisk::task<std::unique_ptr<http_response>> entrance(http_request_wrapper&request);
    };
}

#endif //USER_CONTROLLER_H
