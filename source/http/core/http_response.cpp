//
// Created by hackman on 10/8/23.
//

#include <sstream>
#include "http/core/http_response.h"

#include "http/core/http_iodata_stream_wrapper.h"

namespace obelisk::http {
    std::unordered_map<EResponseCode, std::string> http_response::resp_status_map_{
            {EST_CONTINUE, "Continue"},
            {EST_OK, "OK"},
            {EST_CREATED, "Created"},
            {EST_ACCEPTED, "Accepted"},
            {EST_NO_CONTENT, "No Content"},
            {EST_MOVED_PERMANENTLY, "Moved Permanently"},
            {EST_FOUND, "Found"},
            {EST_NOT_MODIFIED, "Not Modified"},
            {EST_USE_PROXY, "Use Proxy"},
            {EST_TEMPORARY_REDIRECT, "Temporary Redirect"},
            {EST_BAD_REQUEST, "Bad Request"},
            {EST_UNAUTHORIZED, "Unauthorized"},
            {EST_FORBIDDEN, "Forbidden"},
            {EST_NOT_FOUND, "Not Found"},
            {EST_METHOD_NOT_ALLOWED, "Method Not Allowed"},
            {EST_NOT_ACCEPTABLE, "Not Acceptable"},
            {EST_PROXY_AUTHENTICATION_REQUIRED, "Proxy Authentication Required"},
            {EST_REQUEST_TIMEOUT, "Request Timeout"},
            {EST_CONFLICT, "Conflict"},
            {EST_GONE, "Gone"},
            {EST_LENGTH_REQUIRED, "Length Required"},
            {EST_PRECONDITION_FAILED, "Precondition Failed"},
            {EST_REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large"},
            {EST_REQUEST_URI_TOO_LONG, "Request-URI Too Long"},
            {EST_UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"},
            {EST_REQUESTED_RANGE_NOT_SATISFIABLE, "Requested Range Not Satisfiable"},
            {EST_EXPECTATION_FAILED, "Expectation Failed"},
            {EST_UNPROCESSABLE_CONTENT, "Unprocessable Content"},
            {EST_INTERNAL_SERVER_ERROR, "Internal Server Error"},
            {EST_NOT_IMPLEMENTED, "Not Implemented"},
            {EST_BAD_GATEWAY, "Bad Gateway"},
            {EST_SERVICE_UNAVAILABLE, "Service Unavailable"},
            {EST_GATEWAY_TIMEOUT, "Gateway Timeout"},
            {EST_HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"},

    };

    http_response::http_response(EResponseCode code) {
        header_.meta_.p1_ = "HTTP/1.1";
        header_.meta_.p2_ = std::to_string(code);
        header_.meta_.p3_ = resp_status_map_[code];
    }


    std::uint64_t http_response::content_length() {
        return 0;
    }

    sahara::container::unordered_smap_u<std::string>& http_response::headers() {
        return header_.headers_;
    }

    std::unique_ptr<core::http_iodata> http_response::serialize_header() {
        auto ss = std::make_unique<std::stringstream>();
        *ss << header_.meta_.p1_ << " " << header_.meta_.p2_ << " " << header_.meta_.p3_ << "\r\n";
        for(auto &header: header_.headers_){
            *ss << header.first << ": " << header.second << "\r\n";
        }
        *ss << "\r\n";
        return std::make_unique<core::http_data_istream_wrapper>(std::move(ss), ss->str().length());
    }

    std::unique_ptr<core::http_iodata> http_response::serialize() {
        if(body_)
            header_.headers_.emplace("Content-Length", std::to_string(body_->size()));
        auto header = serialize_header();

        auto result = std::make_unique<core::http_multi_source_iodata>();
        result->append(std::move(header));
        if(body_)
            result->append(std::move(body_));

        return result;
    }
} // obelisk