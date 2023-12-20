#include "http/middleware/multipart_extract.h"

namespace obelisk::http::middleware {
    std::unique_ptr<http_response> multipart_extract::pre_handle(http_request_wrapper& request) {
        return nullptr;
    }
} // obelisk::http::middleware