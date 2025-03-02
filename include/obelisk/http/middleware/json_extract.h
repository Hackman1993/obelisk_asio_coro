//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef JSON_EXTRACT_H
#define JSON_EXTRACT_H
#include "middleware.h"

namespace obelisk {
namespace http {
namespace middleware {

    class json_extract : public before_middleware{
    public:
        boost::asio::awaitable<std::unique_ptr<http_response>> pre_handle(http_request_wrapper& request) override;
    };
} // middleware
} // http
} // obelisk

#endif //JSON_EXTRACT_H
