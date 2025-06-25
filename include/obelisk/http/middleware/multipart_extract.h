//
// Created by Hackman.Lo on 2023/12/8.
//

#ifndef MULTIPART_EXTRACT_H
#define MULTIPART_EXTRACT_H
#include "middleware.h"
#include <boost/asio/awaitable.hpp>

namespace obelisk::http::middleware {

class multipart_extract : public base_middleware{
public:
    boost::asio::awaitable<std::unique_ptr<http_response>> pre_handle(http_request_wrapper& request) override;
};

} // obelisk::http::middleware

#endif //MULTIPART_EXTRACT_H
