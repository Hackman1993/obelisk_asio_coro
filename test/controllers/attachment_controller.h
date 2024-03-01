//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef ATTACHMENT_CONTROLLER_H
#define ATTACHMENT_CONTROLLER_H
#include "http/core/http_request.h"

namespace obelisk::http {
    class http_response;
}

boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> upload(obelisk::http::http_request_wrapper&request);
#endif //ATTACHMENT_CONTROLLER_H
