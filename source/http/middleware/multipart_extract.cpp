//
// Created by Hackman.Lo on 2023/12/8.
//

#include "http/middleware/multipart_extract.h"

namespace obelisk::http::middleware {
    std::shared_ptr<http_response> multipart_extract::pre_handle(std::shared_ptr<http_request_wrapper> request) {
        return nullptr;
    }
} // obelisk::http::middleware