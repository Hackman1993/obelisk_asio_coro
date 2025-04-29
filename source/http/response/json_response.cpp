#include "obelisk/http/response/json_response.h"
#include "obelisk/http/core/http_iodata_stream_wrapper.h"
#include <sstream>
#include <nlohmann/json.hpp>

namespace obelisk::http {
    json_response::json_response(const nlohmann::json& json, const EResponseCode code): http_response(code)
    {
        header_.headers_["Content-Type"] = "application/json";
        auto data_str = json.dump();
        auto data = std::make_unique<std::stringstream>(data_str);
        body_ = std::make_unique<core::http_data_istream_wrapper>(std::move(data), data_str.length());
    }
} // obelisk