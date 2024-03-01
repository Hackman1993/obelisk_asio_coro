#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "http/response/json_response.h"
#include <core/coroutine/async_scoped_lock.h>

#include "http/core/http_request.h"

using namespace obelisk::http::validator;
boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> login(obelisk::http::http_request_wrapper&request);
boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> logout(obelisk::http::http_request_wrapper&request);

#endif //AUTH_CONTROLLER_H
