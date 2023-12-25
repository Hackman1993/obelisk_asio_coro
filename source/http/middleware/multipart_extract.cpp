#include "http/middleware/multipart_extract.h"

namespace obelisk::http::middleware {
    boost::asio::awaitable<std::unique_ptr<http_response>> multipart_extract::pre_handle(http_request_wrapper& request) {
        co_return nullptr;
    }
} // obelisk::http::middleware