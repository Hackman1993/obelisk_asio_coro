#include "http/middleware/multipart_extract.h"

#include <boost/algorithm/string/predicate.hpp>

#include "http/exception/http_exception.h"
#include "http/parser/http_parser_v2.h"

namespace obelisk::http::middleware {
    boost::asio::awaitable<std::unique_ptr<http_response>> multipart_extract::pre_handle(http_request_wrapper&request) {
        if(!request.headers().contains("content-type"))
            co_return nullptr;
        std::string content_type = std::string(request.headers()["content-type"]);
        if (const auto&body_stream = request.raw_body(); !body_stream || !boost::algorithm::icontains(
                                                             content_type, "multipart/form-data"))
            co_return nullptr;

        std::string boundary;
        auto result = parser::parse_boundary(content_type, boundary);
        if (!result)
            throw http_exception("error.http.invalid_multipart_data", EResponseCode::EST_UNPROCESSABLE_CONTENT);

        result = parser::parse_multipart_body(request, boundary);
        if (!result)
            throw http_exception("error.http.invalid_multipart_data", EResponseCode::EST_UNPROCESSABLE_CONTENT);

        co_return nullptr;
    }
} // obelisk::http::middleware
