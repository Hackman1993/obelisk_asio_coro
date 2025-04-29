//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef JSON_EXTRACT_H
#define JSON_EXTRACT_H
#include "middleware.h"

namespace obelisk::http::middleware
{
    class json_extract : public base_middleware{
    public:
        boost::asio::awaitable<std::unique_ptr<http_response>> pre_handle(http_request_wrapper& request) override;
    };
}

#endif //JSON_EXTRACT_H
