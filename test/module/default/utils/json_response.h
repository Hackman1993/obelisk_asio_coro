//
// Created by hackman on 4/30/25.
//

#ifndef JSON_RESPONSE_H
#define JSON_RESPONSE_H
#include <boost/asio/awaitable.hpp>
#include <nlohmann/adl_serializer.hpp>
#include <obelisk/http/core/http_response_code.h>

namespace default_::utils
{
    inline boost::asio::awaitable<bool> json_response(const nlohmann::json& json, obelisk::http::EResponseCode)
    {
        std::
        co_return true;
    }
}
#endif //JSON_RESPONSE_H
