//
// Created by hackman on 5/19/25.
//
#include "sys_role.h"

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

    awaitable<std::unique_ptr<http_response>> sys_role::backend_view(http_request_wrapper&request)
    {
        struct role_model
        {
            std::uint64_t id{};
            std::string name;
            //std::optional<std::string> description;
            std::uint64_t fn_organization_id{};
            std::string organization_name;
            std::int64_t user_count{};
        };
        auto admin_org_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto query = db::select({
            {col("r.id"), "id"},
            {col("r.name"), "name"},
            {col("r.fn_organization_id"), "fn_organization_id"},
            {col("o.name"), "organization_name"},
            {count_t(col("ar.fn_admin_id")).distinct(), "user_count"}
        }).from({{"sys_roles", "r"}})
        .join({"sys_organization_hierarchy", "oh"}, {{col("r.fn_organization_id"), col("r.fn_organization_id")}})
        .join({"sys_organizations", "o"}, {
            {col("r.fn_organization_id"), col("o.id")},
            {col("o.deleted_at"), nullptr}
        }).left_join({"sys_mid_admin_role", "ar"}, {{col("r.id"), col("ar.fn_role_id")},
        }).global_where({
            {col("r.deleted_at"), nullptr},
            {col("oh.fn_ancestor_id"), admin_org_id}
        }).group_by({"r.id", "r.name", "r.fn_organization_id", "o.name"}).order_by({{"r.id"}});
        if (!co_await can_cascade("permission.sys_role.view", admin_id))
            query.global_where({{col("r.fn_organization_id"), admin_org_id}});

        if (request.params().contains("search"))
        {
            std::string search = request.params()["search"];
            query.or_where({{col("r.name"), "like", search + "%"}});
        }
        auto result = co_await query.get<role_model>();
        co_return co_await pagination<role_model>(query, co_await pagination_uniform(query, request));
    }



    awaitable<std::unique_ptr<http_response>> sys_role::backend_create(http_request_wrapper&request)
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
        if (!co_await can("permission.role.create", request, fn_organization_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);

        std::vector<std::pair<col, obelisk::database::core::sql_value>> values;
        values.emplace_back("fn_organization_id", fn_organization_id);
        try_emplace<std::string>("name", values, request.params());
        //try_emplace<std::string>("description", values, request.params());

        co_await db::insert("sys_roles").values(values).get();
        co_return json_response(nullptr);
    }
    awaitable<std::unique_ptr<http_response>> sys_role::backend_update(http_request_wrapper&request)
    {
         co_await request.validate({
             {"id", {required(), integer(false)}},
             {"name", {required()}}
        });
        auto target_id = boost::lexical_cast<std::uint64_t>(request.params()["id"].get<std::string>());
        auto result  = co_await db::select({col("fn_organization_id")}).from({"sys_roles"}).where({
            {col("id"), target_id},
            {col("deleted_at"), nullptr}
        }).get();
        if (result.rows().empty())
            throw http_exception("server.error.not_found", EST_NOT_FOUND);
        if (!co_await can("permission.roles.update", request, result.rows()[0][0].as_uint64()))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);
        std::vector<std::pair<col, obelisk::database::core::sql_value>> values;
        try_emplace<std::string>("name", values, request.params());
        try_emplace<std::string>("description", values, request.params());
        co_await db::update({"sys_roles"}).set(values).where({
            {col("id"), target_id}
        }).get();

        co_return json_response(nullptr);
    }
    awaitable<std::unique_ptr<http_response>> sys_role::backend_delete(http_request_wrapper&request)
    {
        co_await request.validate({
            {"id", {required(), integer(false)}},
       });
        auto target_id = boost::lexical_cast<std::uint64_t>(request.params()["id"].get<std::string>());
        auto result  = co_await db::select({col("fn_organization_id")}).from({"sys_roles"}).where({
            {col("id"), target_id},
            {col("deleted_at"), nullptr}
        }).get();
        if (result.rows().empty())
            throw http_exception("server.error.not_found", EST_NOT_FOUND);
        if (!co_await can("permission.sys_role.delete", request, result.rows()[0][0].as_uint64()))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);
        co_await db::update({"sys_roles"}).where({
            {col("deleted_at"), std::chrono::system_clock::now()}
        }).get();
        co_return json_response(nullptr);
    }
}
