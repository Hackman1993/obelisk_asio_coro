#include "sys_organization_controller.h"
#include "module/default/utils/utils.h"
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/integer_validator.h>
#include <obelisk/http/validator/required_validator.h>
#include <obelisk/database/core/common.h>
namespace module::default_::controllers
{
    using namespace obelisk::database::builder::detail;
    awaitable<std::unique_ptr<obelisk::http::http_response>> sys_organization_controller::backend_view(obelisk::http::http_request_wrapper&request)
    {
        struct organization_model
        {
            std::uint64_t id{};
            std::string name;
            std::optional<std::string> director_name;
            std::optional<std::string> director_phone;
            std::optional<std::string> emergency_name;
            std::optional<std::string> emergency_phone;
            std::optional<std::string> address;
            std::optional<std::uint64_t> path_length;
            std::optional<std::uint64_t> parent_id;
        };

        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto query = db::select({
            {col("so.id"), "id"},
            {col("so.name"), "name"},
            {col("so.director_name"), "director_name"},
            {col("so.director_phone"), "director_phone"},
            {col("so.emergency_name"), "emergency_name"},
            {col("so.emergency_phone"), "emergency_phone"},
            {col("so.address"), "address"},
            {col("soh.path_length")},
            {col("sop.id"), "parent_id"},
        }).from({{"sys_organizations", "so"}})
        .join({"sys_organization_hierarchy", "soh"}, {{col("so.id"),col("soh.fn_descendant_id")}})
        .left_join({"sys_organization_hierarchy", "sohp"}, {{col("so.id"), col("sohp.fn_descendant_id")}, {col("sohp.path_length"), 1}})
        .left_join({"sys_organizations", "sop"}, {{col("sohp.fn_ancestor_id"), col("sop.id")}})
        .join({"sys_admins", "sa"}, {{col("sa.fn_organization_id"), col("soh.fn_ancestor_id")}})
        .where({
            {col("sa.id"), admin_id}
        }).order_by({{"soh.path_length", "so.name"}});


        if (!co_await utils::can_cascade("permission.sys_organization.view", admin_id))
            query.where({{col("so.id"), col("sa.fn_organization_id")}});
        auto result = co_await query.get<organization_model>();
        co_return utils::json_response(utils::to_json(result));
    }

    awaitable<std::unique_ptr<obelisk::http::http_response>> sys_organization_controller::backend_create(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::http::validator;
        using namespace obelisk::database::builder::detail;
        co_await request.validate({
            {"parent_id", {required(), integer(false)}},
            {"name", {required()}},
        });
        std::uint64_t parent_id = boost::lexical_cast<std::uint64_t>(request.params()["parent_id"].get<std::string>());
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        if (!co_await utils::can("permission.sys_organization.create", admin_id, parent_id))
            throw obelisk::http::http_exception("server.error.permission_denied", obelisk::http::EST_UNAUTHORIZED);
        auto check_query = db::select({col("id")}).from({"sys_organizations"}).where({
            {col("id"), parent_id},
            {col("deleted_at"), nullptr}
        });
        if (co_await check_query.count() == 0)
            throw obelisk::http::http_exception("server.error.target_not_exists", obelisk::http::EST_UNPROCESSABLE_CONTENT);

        auto& params = request.params();
        std::vector<std::pair<col, core::sql_value>> values{{"name", request.params()["name"].get<std::string>()}};
        try_emplace<std::uint64_t>("parent_id", values, params);
        try_emplace<std::string>("director_name", values, params);
        try_emplace<std::string>("director_phone", values, params);
        try_emplace<std::string>("emergency_name", values, params);
        try_emplace<std::string>("emergency_phone", values, params);
        try_emplace<std::string>("address", values, params);

        co_await db::transaction([&values, parent_id](auto connection)->boost::asio::awaitable<void>
        {
            auto prefix = obelisk::http::config::get<std::string>("database.default.prefix", "");
            auto result = co_await db::insert("sys_organizations").values(values).get(connection);
            auto org_id = result.last_insert_id();
            co_await db::insert("sys_organization_hierarchy", {"fn_ancestor_id", "fn_descendant_id", "path_length"})
            .values({[parent_id, org_id](auto& query){
                query.select({col("fn_ancestor_id"), org_id, raw("path_length+1")})
                .from({"sys_organization_hierarchy"}).where({
                    {col("fn_descendant_id"), parent_id},
                }).u_nion({org_id,org_id,0});
            }}).template get<void>(connection);
            co_return;
        });
        co_return utils::json_response(nullptr);
    }

    awaitable<std::unique_ptr<obelisk::http::http_response>> sys_organization_controller::backend_update(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::database::builder::detail;
        using namespace obelisk::http::validator;
        co_await request.validate({
            {"id", {required(), integer(false)}},
        });
        std::uint64_t target_id = boost::lexical_cast<std::uint64_t>(request.params()["id"].get<std::string>());
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        if (!co_await utils::can("permission.sys_organization.update", admin_id, target_id))
            throw obelisk::http::http_exception("server.error.permission_denied", obelisk::http::EST_UNAUTHORIZED);
        auto check_query = db::select({col("id")}).from({"sys_organizations"}).where({
            {col("id"), target_id},
            {col("deleted_at"), nullptr}
        });
        if (co_await check_query.count() == 0)
            throw obelisk::http::http_exception("server.error.target_not_exists", obelisk::http::EST_UNPROCESSABLE_CONTENT);

        auto& params = request.params();
        std::vector<std::pair<col, core::sql_value>> values;
        try_emplace<std::string>("name", values, params);
        try_emplace<std::string>("director_name", values, params);
        try_emplace<std::string>("director_phone", values, params);
        try_emplace<std::string>("emergency_name", values, params);
        try_emplace<std::string>("emergency_phone", values, params);
        try_emplace<std::string>("address", values, params);
        co_await db::update({"sys_organizations"}).set(values).where({
            {col("id"), target_id}
        }).get();
        co_return utils::json_response(nullptr);
    }

    awaitable<std::unique_ptr<obelisk::http::http_response>> sys_organization_controller::backend_delete(obelisk::http::http_request_wrapper&request)
    {
        using namespace obelisk::database::builder::detail;
        auto target_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        if (!co_await utils::can("permission.sys_organization.delete", admin_id, target_id))
            throw obelisk::http::http_exception("server.error.permission_denied", obelisk::http::EST_UNAUTHORIZED);
        co_await db::transaction([target_id](auto connection)->awaitable<void>
        {
            co_await db::delete_from({"sys_organization_hierarchy"}).where({
                {col("fn_ancestor_id"), target_id}
            }).or_where({
                {col("fn_descendant_id"), target_id}
            }).get(connection);
            co_await db::update({"sys_organizations"}).where({
                {col("id"), target_id}
            }).set({{"deleted_at", std::chrono::system_clock::now()}}).get(connection);
        });
        co_return utils::json_response(nullptr);
    }
}