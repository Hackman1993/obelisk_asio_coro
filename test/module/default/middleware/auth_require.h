//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef MIDDLEWARE_AUTH_REQUIRE_H
#define MIDDLEWARE_AUTH_REQUIRE_H

#include <obelisk/obelisk.h>
#include <obelisk/core/coroutine/task.h>
#include <obelisk/http/exception/http_exception.h>


namespace module::default_::middleware{
    class auth_require : public obelisk::http::middleware::base_middleware {
    public:
        auth_require(std::string target_key, std::vector<std::string> permissions): target_key_(std::move(target_key)), permissions_(std::move(permissions))
        {
        }
        boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> pre_handle(obelisk::http::http_request_wrapper&request) override {
            if (!request.headers().contains("authorization"))
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);


            auto authorization_header = request.headers()["Authorization"];
            if (authorization_header.find("Bearer ") == std::string::npos)
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);
            auto token = authorization_header.substr(authorization_header.find("Bearer ") + 7);

            const boost::mysql::results results = co_await obelisk::database::db::select({"id", "fn_target_id", "target_key"}).from({"sys_access_tokens"}).where({
                {"token", token},
                {"target_key", target_key_},
                {"expires_at", ">", std::chrono::system_clock::now()},
            }).get();

            if (results.rows().empty())
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);

            co_await obelisk::database::db::update({"sys_access_tokens"}).set({
                {"expires_at", std::chrono::system_clock::now() + std::chrono::minutes(30)},
                {"last_used_at", std::chrono::system_clock::now()}
            }).where({
                {"id", results.rows()[0][0].as_uint64()}
            }).get();
            request.additional_data().emplace(std::format("_{}_target_id", target_key_), results.rows()[0][1].as_uint64());

            co_return nullptr;
        }

    private:
        std::string target_key_;
        std::optional<std::vector<std::string>> permissions_;
    };

}


#endif //MIDDLEWARE_AUTH_REQUIRE_H
