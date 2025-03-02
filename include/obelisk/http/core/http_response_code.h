/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_response_code.h
 * @description 
 * @created_at 2023-10-10
***********************************************************************************************************************/

#ifndef OBELISK_HTTP_RESPONSE_CODE_H
#define OBELISK_HTTP_RESPONSE_CODE_H

namespace obelisk::http {

    enum EResponseCode {
        EST_CONTINUE = 100u,
        EST_OK = 200u,
        EST_CREATED = 201u,
        EST_ACCEPTED = 202u,
        EST_NO_CONTENT = 204u,
        EST_MOVED_PERMANENTLY = 301u,
        EST_FOUND = 302u,
        EST_NOT_MODIFIED = 304u,
        EST_USE_PROXY = 305u,
        EST_TEMPORARY_REDIRECT = 307u,
        EST_BAD_REQUEST = 400u,
        EST_UNAUTHORIZED = 401u,
        EST_FORBIDDEN = 403u,
        EST_NOT_FOUND = 404u,
        EST_METHOD_NOT_ALLOWED = 405u,
        EST_NOT_ACCEPTABLE = 406u,
        EST_PROXY_AUTHENTICATION_REQUIRED = 407u,
        EST_REQUEST_TIMEOUT = 408u,
        EST_CONFLICT = 409u,
        EST_GONE = 410u,
        EST_LENGTH_REQUIRED = 411u,
        EST_PRECONDITION_FAILED = 412u,
        EST_REQUEST_ENTITY_TOO_LARGE = 413u,
        EST_REQUEST_URI_TOO_LONG = 414u,
        EST_UNSUPPORTED_MEDIA_TYPE = 415u,
        EST_REQUESTED_RANGE_NOT_SATISFIABLE = 416u,
        EST_EXPECTATION_FAILED = 417u,
        EST_UNPROCESSABLE_CONTENT = 422u,
        EST_INTERNAL_SERVER_ERROR = 500u,
        EST_NOT_IMPLEMENTED = 501u,
        EST_BAD_GATEWAY = 502u,
        EST_SERVICE_UNAVAILABLE = 503u,
        EST_GATEWAY_TIMEOUT = 504u,
        EST_HTTP_VERSION_NOT_SUPPORTED = 505u,
    };

} // obelisk

#endif //OBELISK_HTTP_RESPONSE_CODE_H
