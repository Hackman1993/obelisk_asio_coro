//
// Created by hackman on 4/29/25.
//

#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H
#include <sahara/hash/bcrypt.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/required_validator.h>
#include "module/default/utils/utils.h"
#include <chrono>
#include <iostream>

namespace module::default_::controllers
{
    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_login(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        co_await request.validate({
            {"login", {required()}},
            {"password", {required()}}
        });
        const auto result = co_await obelisk::database::db::select({"id","username", "password", "phone"}).from({"sys_admins"}).where({
            { col("username"), request.params()["login"].get<std::string>()},
            { col("deleted_at"), nullptr}
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

        auto access_token = boost::algorithm::replace_all_copy(sahara::utils::uuid::generate(), "-", "");
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

    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_logout(obelisk::http::http_request_wrapper&request)
    {
        if (!request.headers().contains("Authorization") || !request.headers()["Authorization"].contains("Bearer "))
            co_return utils::json_response(nullptr);

        const auto authorization_header = request.headers()["Authorization"];

        co_await obelisk::database::db::delete_from({"sys_access_tokens"}).where({
            {"token", authorization_header.substr(authorization_header.find("Bearer ")+7)},
            {"fn_target_id", std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_target_id"])},
            {"target_key", "sys_admins"}
        }).get();
        co_return utils::json_response(nullptr);
    }
    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_permissions(obelisk::http::http_request_wrapper&request)
    {
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_target_id"]);
        std::cout << obelisk::database::db::select({
            {"sp.code", "code" },
            {"sp.visible", "visible"}
        }).from({{"sys_admins", "sa"}})
        .inner_join({"sys_mid_admin_role", "smar"}, {{col("sa.id"), col("smar.fn_admin_id")} , {"sa.deleted_at", nullptr}})
        .inner_join({"sys_roles", "sr"}, {{col("smar.fn_role_id"), col("sr.id")}, {"sr.deleted_at", nullptr}})
        .inner_join({"sys_mid_role_permission", "smrp"}, {{col("sr.id"), col("smrp.fn_role_id")}})
        .inner_join({"sys_permissions", "sp"}, {{col("smrp.fn_permission_id"), col("sp.id")}})
        .where({
            {"sa.id", admin_id}
        }).compile()<< std::endl;
        co_return nullptr;
    }
}

#endif //AUTH_CONTROLLER_H
