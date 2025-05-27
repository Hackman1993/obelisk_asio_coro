#ifndef OBELISK_HTTP_PARSER_V3_H
#define OBELISK_HTTP_PARSER_V3_H

#include <string>
#include <memory>
#include <unordered_map>

#include "obelisk/http/core/http_request.h"
#include "obelisk/http/core/raw.h"
#include "boost/spirit/home/x3.hpp"

namespace obelisk::http {
    class url_parts;
    class parser_v3 {
    public:
        static bool parse_boundary(std::string_view data, std::string &boundary);

        static bool parse_http_header(std::string_view data, core::raw::http_header_raw &request);

        static bool parse_body(http_request &request);

        static bool parse_multipart_body(http_request_wrapper &request, const std::string& boundary);

        static bool parse_urlencoded_param(nlohmann::json::object_t& params, std::string_view data);

        static std::unique_ptr<url_parts> parse_split_url(const std::string& uri);
    };

} // obelisk

#endif //OBELISK_HTTP_PARSER_V3_H
