//
// Created by Hackman.Lo on 2024/2/29.
//

#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H
#include "obelisk.h"

using namespace obelisk::http;
class authorization : public middleware::before_middleware {
public:
    boost::cobalt::task<std::unique_ptr<http_response>> pre_handle(http_request_wrapper& request) override;
};



#endif //AUTHORIZATION_H
