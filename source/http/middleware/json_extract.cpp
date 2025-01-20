//
// Created by Hackman.Lo on 2024/3/4.
//

#include "http/middleware/json_extract.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>

#include "http/exception/http_exception.h"


namespace obelisk::http::middleware {
    boost::asio::awaitable<std::unique_ptr<http_response>> json_extract::pre_handle(http_request_wrapper& request) {
        if(!request.headers().contains("content-type") || !boost::algorithm::icontains(request.headers()["content-type"], "application/json"))
            co_return nullptr;
        if(!request.raw_body()) {
            co_return nullptr;
        }
        boost::system::error_code err;
        boost::json::value json_data = boost::json::parse(*request.raw_body(), err);
        if(err) {
            throw http_exception("error.http.invalid_json_data", EResponseCode::EST_UNPROCESSABLE_CONTENT);
        }

        auto json = json_data.as_object();
        for (auto& item: json) {
            if(!request.params().contains(item.key())) {
                request.params().emplace(item.key(), item.value());
            }

            // request.params().contains(item.key())
            // request.params("",item.value().as_string());
            // if(item.value().is_primitive())
        }
        request.additional_data().emplace("__json_body", json_data.as_object());
        co_return nullptr;
    }
} // obelisk::http::middleware