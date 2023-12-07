//
// Created by hackman on 10/9/23.
//

#include "http/response/json_response.h"
#include <sstream>
namespace obelisk::http {
    json_response::json_response(const boost::json::object& object, EResponseCode code) : http_response(code) {
        data_.header_.headers_["Content-Type"] = "application/json";
        std::string data = boost::json::serialize(object);
        data_.content_length_ = data.size();
        data_.content_ = std::make_shared<std::stringstream>(data);
    }

    json_response::json_response(const boost::json::array& object, EResponseCode code) : http_response(code) {
        data_.header_.headers_["Content-Type"] = "application/json";
        std::string data = boost::json::serialize(object);
        data_.content_length_ = data.size();
        data_.content_ = std::make_shared<std::stringstream>(data);
    }
} // obelisk