#ifndef OBELISK_HTTP_PARSER_H
#define OBELISK_HTTP_PARSER_H

#include <string>
#include <memory>
#include <unordered_map>

#include "../core/http_request.h"
#include "http/core/http_block_data.h"
#include "boost/spirit/home/x3.hpp"

namespace obelisk::http {
    class http_block_data;
    class parser {
    public:
        static bool parse_boundary(std::string_view data, std::string &boundary);

        static bool parse_http_header(std::string_view data, http_header &request);

        static bool parse_body(http_request &request);

        static bool parse_multipart_body(http_request_wrapper &request, const std::string& boundary);

        static bool parse_urlencoded_param(http_request_wrapper &request, std::string_view data);
    };

} // obelisk

#endif //OBELISK_HTTP_PARSER_H
