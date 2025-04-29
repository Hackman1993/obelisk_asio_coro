//
// Created by hackman on 4/29/25.
//

#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H
#include <sahara/hash/bcrypt.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/required_validator.h>
#include "module/default/utils/utils.h"

namespace default_::controllers
{
    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> login(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        co_await request.validate({
            {"login", {required()}},
            {"password", {required()}}
        });
        auto result = co_await obelisk::database::db::select({"id","username", "password", "phone"}).from({"sys_admins"}).where({
            { "username", request.params()["login"].get<std::string>()},
            { "deleted_at", nullptr}
        }).get();
        if (result.rows().empty())
            throw obelisk::http::http_exception("server.error.invalid_credential", obelisk::http::EST_UNAUTHORIZED);
        if (!obelisk::http::config::get<bool>("debug", false))
        {
            co_await request.validate({{"verify_code", {required()}}});
            if (!co_await utils::validate_verify_code(result.rows()[0][3].as_string(), request.params()["verify_code"].get<std::string>(), "backend.login"))
                throw obelisk::http::http_exception("server.error.invalid_verify_code", obelisk::http::EST_UNAUTHORIZED);
        }
        if (!sahara::hash::bcrypt::validatePassword(request.params()["password"].get<std::string>(), result.rows()[0][2].as_string()))
            throw obelisk::http::http_exception("server.error.invalid_credential", obelisk::http::EST_UNAUTHORIZED);

        auto access_token = sahara::utils::uuid::generate();
        co_await obelisk::database::db::insert("sys_access_tokens").values({
            {"fn_target_id", result.rows()[0][0].as_uint64()},
            {"target_key", "sys_admins"},
            {"token", access_token},
            {"expires_at", std::chrono::system_clock::now() + std::chrono::hours(2)}
        }).get();
        co_return utils::json_response(nlohmann::json::object({
            {"access_token", access_token}
        }));
    }

    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> permissions(obelisk::http::http_request_wrapper&request)
    {
        co_return nullptr;
    }
}

#endif //AUTH_CONTROLLER_H
