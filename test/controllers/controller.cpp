//
// Created by hackman on 5/19/25.
//

#include "controller.h"

#include <boost/lexical_cast.hpp>
#include <obelisk/http/exception/http_exception.h>

namespace controllers
{
    boost::asio::awaitable<bool> controller::can_cascade(const std::string& code,std::uint64_t sys_admin_id)
    {
        using namespace obelisk::database::builder::detail;
        auto query = db::select({col("sp.id")}).from({{"sys_mid_role_permission", "smrp"}})
        .left_join({"sys_roles", "sr"},{{col("smrp.fn_role_id"), col("sr.id")},{col("sr.deleted_at"), nullptr}})
        .left_join({"sys_permissions", "sp"}, {{col("sp.id"), col("smrp.fn_permission_id")}})
        .left_join({"sys_mid_admin_role", "smar"}, {{col("smar.fn_role_id"), col("sr.id")}})
        .left_join({"sys_admins", "sa"}, {{col("sa.id"), col("smar.fn_admin_id")}, {col("sa.deleted_at"), nullptr}})
        .left_join({"sys_organizations", "so"}, {{col("so.id"), col("sa.fn_organization_id")}, {col("so.deleted_at"), nullptr}})
        .where({
            {col("sa.id"), sys_admin_id},
            {col("sp.code"), code},
            {col("smrp.cascade"), 1},
        }).order_by({{"smrp.cascade"}, "DESC"}).limit(1);

        const auto result = co_await query.get();
        co_return !result.rows().empty();
    }

    std::unique_ptr<obelisk::http::json_response> controller::json_response(const nlohmann::json& json, const obelisk::http::EResponseCode code)
    {
        return std::make_unique<obelisk::http::json_response>(nlohmann::json{
            {"data", json},
            {"code", code}
        }, code);
    }

    awaitable<std::uint64_t> controller::can(const std::string& code, obelisk::http::http_request_wrapper& request, const std::string& model, std::uint64_t target_id)
    {
        using namespace obelisk::database::builder::detail;
        struct model_struct
        {
            std::uint64_t fn_organization_id;
        };
        auto models = co_await db::select({col("fn_organization_id")}).from({std::format("{}s", model)}).where({
            {col("id"), target_id},
            {col("deleted_at"), nullptr}
        }).get<model_struct>();
        if (models.rows().empty())
            throw obelisk::http::http_exception("server.error.not_found", obelisk::http::EST_NOT_FOUND);

        auto target_org_id = models.rows()[0].fn_organization_id;
        if (co_await can(code, request, models.rows()[0].fn_organization_id))
            co_return target_org_id;
        throw obelisk::http::http_exception("server.error.permission_denied", obelisk::http::EST_UNAUTHORIZED);
    }

    awaitable<bool> controller::can(const std::string& code, obelisk::http::http_request_wrapper& request, std::uint64_t target_org_id)
    {
        using namespace obelisk::database::builder::detail;

        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        auto query = db::select({col("sa.fn_organization_id"),col("smrp.cascade")}).from({{"sys_mid_role_permission", "smrp"}})
        .join({"sys_roles", "sr"},{{col("smrp.fn_role_id"), col("sr.id")},{col("sr.deleted_at"), nullptr}})
        .join({"sys_permissions", "sp"}, {{col("sp.id"), col("smrp.fn_permission_id")}})
        .join({"sys_mid_admin_role", "smar"}, {{col("smar.fn_role_id"), col("sr.id")}})
        .join({"sys_admins", "sa"}, {{col("sa.id"), col("smar.fn_admin_id")}, {col("sa.deleted_at"), nullptr}})
        .join({"sys_organization_hierarchy", "soh"}, {
            {col("soh.fn_ancestor_id"), col("sa.fn_organization_id")},
            {col("soh.fn_ancestor_id"), col("sr.fn_organization_id")}
        })
        .where({
            {col("sa.id"), admin_id},
            {col("sp.code"), code},
            {col("soh.fn_descendant_id"), target_org_id},
            {col("soh.fn_ancestor_id"), col("sa.fn_organization_id")},
        }).order_by({{"smrp.cascade"}, "DESC"}).limit(1);
        const auto result = co_await query.get();
        if (result.rows().empty())
            co_return false;

        if (target_org_id != result.rows()[0][0].as_uint64())
            co_return result.rows()[0][1].as_int64() == 1;
        co_return true;
    }


    awaitable<pagination_uniformed> controller::pagination_uniform(builder::detail::query statement, obelisk::http::http_request_wrapper& request)
    {
        auto count_query = statement;
        auto total = co_await count_query.count();
        std::uint64_t limit = 10;
        std::uint64_t page = 1;
        if (request.params().contains("page"))
        {
            try
            {
                page = boost::lexical_cast<std::uint64_t>(request.params()["page"].get<std::string>());
            }catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        if (request.params().contains("limit"))
        {
            try
            {
                limit = boost::lexical_cast<std::uint64_t>(request.params()["limit"].get<std::string>());
            }catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        const std::uint64_t max_page = std::ceil(total/limit);
        page = std::min(page, max_page);
        page = std::max<std::uint64_t>(1, page);
        limit = std::max<std::uint64_t>(1, limit);
        co_return pagination_uniformed{total, page, limit};
    }
}
