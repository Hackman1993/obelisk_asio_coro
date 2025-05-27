//
// Created by hackman on 5/19/25.
//
#include "sys_role.h"

#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/confirmed_validator.h>
#include <obelisk/http/validator/required_validator.h>
#include <obelisk/http/validator/integer_validator.h>
#include <obelisk/http/validator/array_validator.h>

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
        .join({"sys_organization_hierarchy", "oh"}, {{col("r.fn_organization_id"), col("oh.fn_descendant_id")}})
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
        co_return co_await pagination<role_model>(query, co_await pagination_uniform(query, request));
    }

    awaitable<std::unique_ptr<http_response>> sys_role::backend_create(http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        co_await request.validate({
            {"name", {required()}},
            {"fn_organization_id", {integer(false)}}
        });

        auto fn_organization_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
        if (request.params().contains("fn_organization_id"))
            fn_organization_id = request.params()["fn_organization_id"];
        if (!co_await can("permission.sys_role.create", request, fn_organization_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);

        std::vector<std::pair<col, sql_value>> values;
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
        if (!co_await can("permission.sys_role.update", request, "sys_role", target_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);
        std::vector<std::pair<col, sql_value>> values;
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
        if (!co_await can("permission.sys_role.delete", request, "sys_role", target_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);
        co_await db::update({"sys_roles"}).set({
            {col("deleted_at"), std::chrono::system_clock::now()}
        }).where({
            {col("id"), target_id}
        }).get();
        co_return json_response(nullptr);
    }

    awaitable<std::unique_ptr<http_response>> sys_role::backend_assignable_permission(http_request_wrapper&request)
    {
        co_await request.validate({
            {"id", {required(), integer(false)}},
        });
        struct permission_model
        {
            std::uint64_t id{};
            std::string code;
            std::int64_t already_own{};
            std::int64_t is_grant{};
            std::int64_t is_cascade{};
            std::int64_t can_grant{};
            std::int64_t can_cascade{};
        };

        auto target_id = boost::lexical_cast<std::uint64_t>(request.params()["id"].get<std::string>());
        if (!co_await can("permission.sys_role.assign_permissions", request, "sys_role", target_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);

        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto query = db::select({
            {col("p.id"), "id"},
            {col("p.code"), "code"},
            {if_format(0,1).where({{col("rp.fn_role_id"), nullptr}}), "already_own"},
            {if_format(0,1).where({{col("rp.grant"),nullptr}}).or_where({{col("rp.grant"),0}}), "is_grant"},
            {if_format(0,1).where({{col("rp.cascade"),nullptr}}).or_where({{col("rp.cascade"),0}}), "is_cascade"},
            {if_format(0,1).where({{col("ap.id"),nullptr}}), "can_grant"},
            {if_format(0,1).where({{col("ap.cascade"),nullptr}}).or_where({{col("ap.cascade"),0}}), "can_cascade"}
        }).from({{"sys_permissions", "p"}})
        .left_join({"sys_mid_role_permission", "rp"}, {{col("p.id"), col("rp.fn_permission_id")}, {col("rp.fn_role_id"), target_id}})
        .left_join({sub_query{[admin_id](auto& builder){
            builder.select({
                {col("rp.fn_permission_id"), "id"},
                {col{"rp.cascade"}, "cascade"}
            })
            .from({{"sys_mid_role_permission", "rp"}})
            .join({"sys_mid_admin_role", "ar"}, {{col("rp.fn_role_id"), col("ar.fn_role_id")},{col("ar.fn_admin_id"), admin_id}})
            .join({"sys_roles", "r"}, {{col("ar.fn_role_id"), col("r.id")}, {col("r.deleted_at"), nullptr}})
            .where({{col("rp.grant"), 1}});
        }}, "ap"}, {{col("ap.id"), col("p.id")}}).order_by({{"p.code", "p.id"}});
        auto resp_data = co_await query.get<permission_model>();
        co_return json_response(to_json(resp_data));
    }

    awaitable<std::unique_ptr<http_response>> sys_role::backend_assign_permission(http_request_wrapper&request)
    {
        co_await request.validate({
            {"id", {required(), integer(false)}},
            {"permissions", {required(), array()}},
            {"ungrant_permissions", {array()}}
        });
        auto target_id = request.params()["id"].get<std::uint64_t>();
        if (!co_await can("permission.sys_role.assign_permissions", request, "sys_role", target_id))
            throw http_exception("server.error.permission_denied", EST_UNAUTHORIZED);

        struct permission_model
        {
            std::uint64_t id{};
            std::string code;
            std::int64_t already_own{};
            std::int64_t is_grant{};
            std::int64_t is_cascade{};
            std::int64_t can_grant{};
            std::int64_t can_cascade{};
        };
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto query = db::select({
            {col("p.id"), "id"},
            {col("p.code"), "code"},
            {if_format(0,1).where({{col("rp.fn_role_id"), nullptr}}), "already_own"},
            {if_format(0,1).where({{col("rp.grant"),nullptr}}).or_where({{col("rp.grant"),0}}), "is_grant"},
            {if_format(0,1).where({{col("rp.cascade"),nullptr}}).or_where({{col("rp.cascade"),0}}), "is_cascade"},
            {if_format(0,1).where({{col("ap.id"),nullptr}}), "can_grant"},
            {if_format(0,1).where({{col("ap.cascade"),nullptr}}).or_where({{col("ap.cascade"),0}}), "can_cascade"}
        }).from({{"sys_permissions", "p"}})
        .left_join({"sys_mid_role_permission", "rp"}, {{col("p.id"), col("rp.fn_permission_id")}, {col("rp.fn_role_id"), target_id}})
        .join({sub_query{[admin_id](auto& builder){
            builder.select({
                {col("rp.fn_permission_id"), "id"},
                {col{"rp.cascade"}, "cascade"}
            })
            .from({{"sys_mid_role_permission", "rp"}})
            .join({"sys_mid_admin_role", "ar"}, {{col("rp.fn_role_id"), col("ar.fn_role_id")},{col("ar.fn_admin_id"), admin_id}})
            .join({"sys_roles", "r"}, {{col("ar.fn_role_id"), col("r.id")}, {col("r.deleted_at"), nullptr}})
            .where({{col("rp.grant"), 1}});
        }}, "ap"}, {{col("ap.id"), col("p.id")}})
        .order_by({{"p.code", "p.id"}});
        auto operatable_permissions = co_await query.get<permission_model>();
        std::map<std::uint64_t, permission_model> permissions;
        for (auto model: operatable_permissions.rows())
            permissions.emplace(model.id, model);

        if (request.params().contains("permissions")){
            for (auto &grant_permission: request.params()["permissions"].items())
            {
                auto permission_id = grant_permission.value()["id"].get<int64_t>();
                if (!permissions.contains(permission_id))
                    continue;
                auto &model = permissions[permission_id];
                auto allow_grant = grant_permission.value().contains("regrant")? grant_permission.value()["regrant"].get<std::int64_t>(): 0ull;
                auto allow_cascade = grant_permission.value().contains("cascade")&& model.can_cascade? grant_permission.value()["cascade"].get<std::int64_t>(): 0ull;
                if (!model.already_own)
                {
                    co_await db::insert("sys_mid_role_permission").values({
                        {"fn_role_id", target_id},
                        {"fn_permission_id", permission_id},
                        {"grant", allow_grant},
                        {"cascade", allow_cascade}
                    }).get();
                }else
                {
                    bool require_execute = false;
                    auto update = db::update({"sys_mid_role_permission"}).where({
                    {col("fn_role_id"), target_id},
                        {col("fn_permission_id"), permission_id}});
                    if (model.can_cascade && allow_cascade != model.is_cascade)
                    {
                        update.set({{"cascade", allow_cascade}});
                        require_execute = true;
                    }
                    if (model.can_grant && allow_grant != model.is_grant)
                    {
                        update.set({{"grant", allow_grant}});
                        require_execute = true;
                    }
                    if (require_execute)
                        co_await update.get();
                }
            }
        }
        // 取消授权
        if (request.params().contains("ungrant_permissions"))
        {
            std::vector<sql_value> ungrant_permissions;
            for (auto &ungrant_permission: request.params()["ungrant_permissions"].items())
            {
                auto permission_id = ungrant_permission.value().get<std::int64_t>();
                if (!permissions.contains(permission_id))
                    continue;
                ungrant_permissions.emplace_back(permission_id);
            }
            if (!ungrant_permissions.empty()){
                co_await db::delete_from({"sys_mid_role_permission"}).where({
                    {col("fn_role_id"), target_id},
                    {col("fn_permission_id"), "IN",  ungrant_permissions}
                }).get();
            }
        }

        co_return json_response(nullptr);
    }
}
