//
// Created by Hackman.Lo on 2023/12/8.
//

#ifndef MULTIPART_EXTRACT_H
#define MULTIPART_EXTRACT_H
#include "middleware.h"

namespace obelisk::http::middleware {

class multipart_extract : public before_middleware{
public:
    std::shared_ptr<http_response> pre_handle(std::shared_ptr<http_request_wrapper> request) override;
};

} // obelisk::http::middleware

#endif //MULTIPART_EXTRACT_H
