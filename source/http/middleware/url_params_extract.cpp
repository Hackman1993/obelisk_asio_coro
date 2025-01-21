//
// Created by Hackman.Lo on 2023/12/22.
//
#include <algorithm>
#include <sahara/string/string_ext.h>
#include "http/middleware/url_params_extract.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/spirit/home/x3.hpp>
#include "http/parser/http_parser_v2.h"
#include "http/exception/protocol_exception.h"

namespace obelisk::http::middleware {
    boost::cobalt::task<std::unique_ptr<http_response>> url_params_extract::pre_handle(http_request_wrapper&request) {
        const auto query = sahara::string_ext::url_decode(request.query_string());
        if (!query.empty()) {
            if (!parser::parse_urlencoded_param(request, query)) {
                throw protocol_exception("UrlEncodedData Parse Failed!");
            }
        }

        if(!request.headers().contains("content_type"))
            co_return nullptr;

        std::string content_type = std::string(request.headers()["content-type"]);
        boost::algorithm::trim(content_type);
        if(const auto &body_stream = request.raw_body();body_stream && boost::algorithm::iequals(content_type, "application/x-www-form-urlencoded")) {
            std::string body_str;
            const std::istream_iterator<unsigned char> beg_itr(*body_stream), end_itr;
            std::copy(beg_itr,end_itr,std::back_inserter(body_str));
            if (!body_str.empty()) {
                if (!parser::parse_urlencoded_param(request, body_str)) {
                    throw protocol_exception("UrlEncodedData Parse Failed!");
                }
            }
        }


        co_return nullptr;
    }
} // obelisk::http::middleware
