//
// Created by hackman on 4/30/25.
//

#ifndef DEFAULT_UTILS_H
#define DEFAULT_UTILS_H
#include <boost/asio/awaitable.hpp>
#include <nlohmann/json.hpp>
#include <obelisk/http/response/json_response.h>

namespace default_
{
    class utils
    {
    public:
        static std::unique_ptr<obelisk::http::json_response> json_response(const nlohmann::json& json, const obelisk::http::EResponseCode code = obelisk::http::EST_OK)
        {
            return std::make_unique<obelisk::http::json_response>(nlohmann::json{
                {"data", json},
                {"code", code}
            }, code);
        }

        static boost::asio::awaitable<bool> validate_verify_code(const std::string& phone, const std::string& verify_code, const std::string& type)
        {
            co_return true;
        }
    };

}
#endif //DEFAULT_UTILS_H
