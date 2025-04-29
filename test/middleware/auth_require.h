//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef MIDDLEWARE_AUTH_REQUIRE_H
#define MIDDLEWARE_AUTH_REQUIRE_H

#include <obelisk/obelisk.h>
#include <obelisk/core/coroutine/task.h>
#include <obelisk/http/exception/http_exception.h>


namespace middleware{
    class auth_require : public obelisk::http::middleware::base_middleware {
    public:
        boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> pre_handle(obelisk::http::http_request_wrapper&request) override {
            if (!request.headers().contains("authorization"))
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);

            auto token = request.headers()["authorization"];
            boost::mysql::results results = co_await obelisk::database::db::select({"fn_target_id", "target_key"}).where({
                {"token", token},
                {"target_key", target_key_}
            }).get();
            if (results.rows().empty())
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);
            request.additional_data().emplace(std::format("_{}_target_id", target_key_), results.rows()[0][0].as_int64());

            co_return nullptr;
        }

    private:
        std::string target_key_;
        std::optional<std::vector<std::string>> permissions_;
    };

}


#endif //MIDDLEWARE_AUTH_REQUIRE_H
