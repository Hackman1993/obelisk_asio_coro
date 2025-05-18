//
// Created by hackman on 4/29/25.
//

#ifndef MODULE_DEFAULT_CONTROLLER_AUTH_CONTROLLER_H
#define MODULE_DEFAULT_CONTROLLER_AUTH_CONTROLLER_H
#include "sahara/hash/bcrypt.h"
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/required_validator.h>
#include "module/default/utils/utils.h"
#include <chrono>
#include <iostream>
#include <bits/random.h>
#include <boost/mysql/pfr.hpp>
#include <clients/aliyun_sms_client.h>
#include <module/default/model/backend_user_info.h>

#include "obelisk/http/validator/in_validator.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <obelisk/database/db.h>
#include <obelisk/database/builder/detail/condition.h>

namespace module::default_::controllers
{
    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_login(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        using namespace obelisk::database::builder::detail;
        co_await request.validate({
            {"login", {required()}},
            {"password", {required()}}
        });
        const auto result = co_await obelisk::database::db::select({col("id"),col("username"), col("password"), col("phone")}).from({"sys_admins"}).where({
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

    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_user_info(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::database::builder::detail;
        struct user_info
        {
            std::uint64_t id{};
            std::string username;
            std::string organization_name;
            std::uint64_t organization_id{};
        };
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        const auto result = co_await obelisk::database::db::select({
            {col("sa.id"), "id"}, {col("sa.username"), "username"}, {col("so.name"), "organization_name"}, {col("sa.fn_organization_id"), "organization_id"}
        }).from({{"sys_admins", "sa"}}).join({table{"sys_organizations"}, "so"},{
            {col{"sa.fn_organization_id"}, col{"so.id"}}, {col{"so.deleted_at"}, nullptr}
        }).where({
            {col{"sa.id"}, admin_id},
            {col{"sa.deleted_at"}, nullptr }
        }).get<user_info>();
        if (result.rows().empty())
            co_return utils::json_response(nullptr);
        co_return utils::json_response(utils::to_json(result.rows()[0]));
    }

    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_logout(obelisk::http::http_request_wrapper&request)
    {
        if (!request.headers().contains("Authorization") || !request.headers()["Authorization"].contains("Bearer "))
            co_return utils::json_response(nullptr);

        const auto authorization_header = request.headers()["Authorization"];

        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        co_await obelisk::database::db::delete_from({"sys_access_tokens"}).where({
            {"token", authorization_header.substr(authorization_header.find("Bearer ")+7)},
            {"fn_target_id", admin_id},
            {"target_key", "sys_admins"}
        }).get();
        co_return utils::json_response(nullptr);
    }
    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_permissions(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::database::builder::detail;
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto organization_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
        struct  permission_model
        {
            std::uint64_t id{};
            std::string code;
            bool is_cascade{};
            bool is_grant{};
        };
        auto query = obelisk::database::db::select({
            {col("sp.id"), "id"},
            {col("sp.code"), "code" },
            {col("smrp.cascade"), "is_cascade"},
            {col("smrp.grant"), "is_grant"}
        }).from({{"sys_admins", "sa"}})
        .join({"sys_mid_admin_role", "smar"}, {{col("sa.id"), col("smar.fn_admin_id")}, {col("sa.fn_organization_id"), organization_id}, {col("sa.deleted_at"), nullptr}})
        .join({"sys_roles", "sr"}, {{col("smar.fn_role_id"), col("sr.id")}, {col("sr.fn_organization_id"), organization_id}, {col("sr.deleted_at"), nullptr}})
        .join({"sys_mid_role_permission", "smrp"}, {{col("sr.id"), col("smrp.fn_role_id")}})
        .join({"sys_permissions", "sp"}, {{col("smrp.fn_permission_id"), col("sp.id")}})
        .where({
            {col("sa.id"), admin_id}
        });

        auto result = co_await query.get<permission_model>();


        co_return utils::json_response(utils::to_json(result));
    }

    inline boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> send_sms(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        using namespace obelisk::database::builder::detail;
        auto avail_type = obelisk::http::config::get<std::vector<std::string>>("sms.channel.verify_code.available_type", {});
        co_await request.validate({
            {"phone", {required()}},
            {"reason", {required(), in(avail_type)}}
        });

        const std::string& phone = request.params()["phone"].get<std::string>();
        const std::string& type = request.params()["reason"].get<std::string>();
        auto query = obelisk::database::db::select({col("id")}).from({"sys_verify_codes"}).where({
            {col("phone"), phone},
            {col("type"), type},
            {col("created_at"), ">", std::chrono::system_clock::now() - std::chrono::seconds(55)}
        });

        if (co_await query.count() >= 1)
            throw obelisk::http::http_exception("server.error.verify_code_too_frequent", obelisk::http::EST_TOO_MANY_REQUEST);

        boost::random::random_device rng;
        boost::random::uniform_int_distribution<> index_dist(0, 999999);
        std::string verify_code = std::format("{:06d}", index_dist(rng));
        auto insert = obelisk::database::db::insert("sys_verify_codes").values({
            {"phone", phone},
            {"type", type},
            {"code", verify_code},
            {"expires_at", std::chrono::system_clock::now() + std::chrono::seconds(300)}
        });
        co_await insert.get();
        co_await utils::send_sms_by_channel("verify_code", phone, nlohmann::json::object_t{{"code", verify_code}});
        co_return utils::json_response(nullptr);
    }
}

#endif //MODULE_DEFAULT_CONTROLLER_AUTH_CONTROLLER_H
