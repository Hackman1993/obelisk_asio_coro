#include "http/response/json_response.h"
#include "http/core/http_iodata_stream_wrapper.h"
#include <sstream>

namespace obelisk::http {
    json_response::json_response(const boost::json::object& object, EResponseCode code) : http_response(code) {
        header_.headers_["Content-Type"] = "application/json";
        auto data_str = boost::json::serialize(object);
        auto data = std::make_unique<std::stringstream>(data_str);
        body_ = std::make_unique<core::http_data_istream_wrapper>(std::move(data), data_str.length());
        // data_.content_length_ = data.size();
        // data_.content_ = std::make_shared<std::stringstream>(data);
    }

    json_response::json_response(const boost::json::array& object, EResponseCode code) : http_response(code) {
        header_.headers_["Content-Type"] = "application/json";
        auto data_str = boost::json::serialize(object);
        auto data = std::make_unique<std::stringstream>(data_str);
        body_ = std::make_unique<core::http_data_istream_wrapper>(std::move(data), data_str.length());
        // data_.content_length_ = data.size();
        // data_.content_ = std::make_shared<std::stringstream>(data);
    }
} // obelisk