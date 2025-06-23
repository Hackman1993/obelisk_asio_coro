//
// Created by hackman on 6/15/25.
//

#include "legacy.h"

namespace module::crown_plaza::controllers
{
    boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> legacy::get_terminal_token(obelisk::http::http_request_wrapper& request)
    {
        co_return std::make_unique<obelisk::http::json_response>(nlohmann::json::object_t{
            {"message", ""},
            {"status_code", 0},
            {"data", nlohmann::json::object_t{
                {"terminal","Client Terminal"},
                {"access_token", sahara::utils::uuid::generate()}
            }}
        });
    }

    boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> legacy::test(obelisk::http::http_request_wrapper& request)
    {
        co_return json_response(nullptr);
    }
}
