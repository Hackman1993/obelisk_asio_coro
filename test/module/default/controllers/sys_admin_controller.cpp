//
// Created by hackman on 5/19/25.
//
#include "sys_admin_controller.h"

#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/confirmed_validator.h>
#include <obelisk/http/validator/required_validator.h>
#include <obelisk/http/validator/integer_validator.h>

#include "auth_controller.h"

namespace module::default_::controllers
{
    using namespace boost::asio;
    using namespace obelisk::database::builder::detail;
    using namespace obelisk::database;
    using namespace obelisk::http;
    using namespace obelisk::http::validator;

    awaitable<std::unique_ptr<http_response>> sys_admin_controller::backend_view(http_request_wrapper&request)
    {
        struct admin_model
        {
            std::uint64_t id{};
            std::string username;
            std::optional<std::string> real_name;
            std::optional<std::string> phone;
            std::uint64_t fn_organization_id{};
            std::string organization_name;
            std::optional<std::string> roles;
        };
        auto admin_org_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto query = db::select({
            {col("sa.id"), "id"},
            {col("sa.username"), "username"},
            {col("sa.real_name"), "real_name"},
            {col("sa.phone"), "phone"},
            {col("sa.fn_organization_id"), "fn_organization_id"},
            {col("so.name"), "organization_name"},
            {group_concat("sr.name").distinct(), "roles"}
        }).from({{"sys_admins", "sa"}})
        .join({"sys_mid_admin_role", "smar"}, {{col("smar.fn_admin_id"), col("sa.id")}})
        .join({"sys_roles", "sr"}, {
            {col("smar.fn_role_id"), col("sr.id")},
            {col("sr.deleted_at"), nullptr}
        }).join({"sys_organization_hierarchy", "soh"}, {{col("sa.fn_organization_id"), col("soh.fn_descendant_id")},
        }).join({"sys_organizations", "so"}, {
            {col("soh.fn_descendant_id"), col("so.id")},
            {col("so.deleted_at"), nullptr}
        }).global_where({
            {col("sa.deleted_at"), nullptr},
            {col("soh.fn_ancestor_id"), admin_org_id}
        }).group_by("sa.id").order_by({{"sa.id"}});
        if (!co_await can_cascade("permission.sys_admin.view", admin_id))
            query.global_where({{col("sa.fn_organization_id"), admin_org_id}});

        if (request.params().contains("search"))
        {
            std::string search = request.params()["search"];
            query.or_where({{col("sa.username"), "like", search + "%"}});
            query.or_where({{col("sa.real_name"), "like", search + "%"}});
            query.or_where({{col("sa.phone"), "like", search + "%"}});
        }
        auto result = co_await query.get<admin_model>();
        co_return co_await pagination<admin_model>(query, co_await pagination_uniform(query, request));
    }



    awaitable<std::unique_ptr<http_response>> sys_admin_controller::backend_create(http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        co_await request.validate({
            {"username", {required()}},
            {"fn_organization_id", {integer(false)}},
            {"password", {required(), confirmed()}},
            {"real_name", {required()}},
            {"phone", {required()}},
        });

        auto fn_organization_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
        if (request.params().contains("fn_organization_id"))
            fn_organization_id = request.params()["fn_organization_id"];
        if (!co_await can("permission.sys_admin.create", request, fn_organization_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);

        std::vector<std::pair<col, obelisk::database::core::sql_value>> values;
        values.emplace_back("fn_organization_id", fn_organization_id);
        try_emplace<std::string>("username", values, request.params());
        values.emplace_back("password", sahara::hash::bcrypt::generateHash(request.params()["password"].get<std::string>()));
        try_emplace<std::string>("real_name", values, request.params());
        try_emplace<std::string>("phone", values, request.params());

        co_await db::insert("sys_admins").values(values).get();
        co_return json_response(nullptr);
    }
    awaitable<std::unique_ptr<http_response>> sys_admin_controller::backend_update(http_request_wrapper&request)
    {
         co_await request.validate({
             {"id", {required(), integer(false)}},
             {"username", {required()}},
             {"password", {confirmed()}},
             {"real_name", {required()}},
             {"phone", {required()}},
        });
        auto target_id = boost::lexical_cast<std::uint64_t>(request.params()["id"].get<std::string>());
        auto result  = co_await db::select({col("fn_organization_id")}).from({"sys_admins"}).where({
            {col("id"), target_id},
            {col("deleted_at"), nullptr}
        }).get();
        if (result.rows().empty())
            throw http_exception("server.error.not_found", EST_NOT_FOUND);
        if (!co_await can("permission.sys_admin.update", request, result.rows()[0][0].as_uint64()))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);
        std::vector<std::pair<col, obelisk::database::core::sql_value>> values;
        try_emplace<std::string>("real_name", values, request.params());
        try_emplace<std::string>("phone", values, request.params());
        co_await db::update({"sys_admins"}).set(values).where({
            {col("id"), target_id}
        }).get();

        co_return json_response(nullptr);
    }
    awaitable<std::unique_ptr<http_response>> sys_admin_controller::backend_delete(http_request_wrapper&request)
    {
        co_await request.validate({
            {"id", {required(), integer(false)}},
       });
        auto target_id = boost::lexical_cast<std::uint64_t>(request.params()["id"].get<std::string>());
        auto result  = co_await db::select({col("fn_organization_id")}).from({"sys_admins"}).where({
            {col("id"), target_id},
            {col("deleted_at"), nullptr}
        }).get();
        if (result.rows().empty())
            throw http_exception("server.error.not_found", EST_NOT_FOUND);
        if (!co_await can("permission.sys_admin.delete", request, result.rows()[0][0].as_uint64()))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);
        co_await db::update({"sys_admins"}).where({
            {col("deleted_at"), std::chrono::system_clock::now()}
        }).get();
        co_return json_response(nullptr);
    }
}
