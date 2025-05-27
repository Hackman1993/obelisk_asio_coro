//
// Created by Hackman.Lo on 2024/3/4.
//

#include "obelisk/http/middleware/json_extract.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <nlohmann/json.hpp>
#include "obelisk/http/exception/http_exception.h"


namespace obelisk::http::middleware {
    obelisk::task<std::unique_ptr<http_response>> json_extract::pre_handle(http_request_wrapper& request) {
        if(!request.headers().contains("content-type") || !boost::algorithm::icontains(request.headers()["content-type"], "application/json"))
            co_return nullptr;
        if(!request.raw_body()) {
            co_return nullptr;
        }

        nlohmann::json json = nlohmann::json::parse(*request.raw_body());
        for (auto &item: json.items())
        {
            request.params().emplace(item.key(), item.value());
        }
        co_return nullptr;
    }
} // obelisk::http::middleware