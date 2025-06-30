//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef CORS_H
#define CORS_H

#include <obelisk/obelisk.h>
#include <obelisk/core/coroutine/task.h>


namespace middleware{
    class cors : public obelisk::http::middleware::base_middleware {
    public:
        obelisk::task<void> after_handle(obelisk::http::http_request_wrapper&request, obelisk::http::http_response&response) override {
            response.headers().emplace("Access-Control-Allow-Origin", "http://localhost:3000");
            response.headers().emplace("Access-Control-Allow-Headers", "Content-Type, Authorization");
            response.headers().emplace("Access-Control-Allow-Credentials", "true");
            co_return;
        }
    };

}


#endif //CORS_H
