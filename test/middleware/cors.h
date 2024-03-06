//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef CORS_H
#define CORS_H

#include <obelisk.h>
using namespace obelisk::http;

class cors : public middleware::after_middleware {
public:
    boost::cobalt::task<void> after_handle(http_request_wrapper&request, http_response&response) override {
        response.headers().emplace("Access-Control-Allow-Origin", "*");
        response.headers().emplace("Access-Control-Allow-Headers", "Content-Type, Authorization");
        co_return;
    }
};


#endif //CORS_H
