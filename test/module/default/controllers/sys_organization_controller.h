//
// Created by hackman on 5/15/25.
//

#ifndef SYS_ORGANIZATION_CONTROLLER_H
#define SYS_ORGANIZATION_CONTROLLER_H
#include <obelisk/database/db.h>
#include <obelisk/database/builder/base_statement.h>

#include "module/default/utils/utils.h"
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/integer_validator.h>
#include <obelisk/http/validator/required_validator.h>

namespace module::default_
{
    class sys_organization_controller {
    public:
        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_view(obelisk::http::http_request_wrapper&request)
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

            auto org_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
            auto query = obelisk::database::db::select({
                col{"so.id", "id"},
                col{"so.name", "name"},
                col{"so.director_name", "director_name"},
                col{"so.director_phone", "director_phone"},
                col{"so.emergency_name", "emergency_name"},
                col{"so.emergency_phone", "emergency_phone"},
                col{"so.address", "address"},
                col{"soh.path_length"},
                col{"sop.id", "parent_id"},
            }).from({{"sys_organizations", "so"}})
            .inner_join({"sys_organization_hierarchy", "soh"}, {{col("so.id"),col("soh.fn_descendant_id")}})
            .left_join({"sys_organization_hierarchy", "sohp"}, {{col("so.id"), col("sohp.fn_descendant_id")}, {col("sohp.path_length"), 1}})
            .left_join({"sys_organizations", "sop"}, {{col("sohp.fn_ancestor_id"), col("sop.id")}})
            .where({
                {col("soh.fn_ancestor_id"), org_id}
            }).order_by({"soh.path_length", "so.name"});

            auto result = co_await query.get<organization_model>();
            co_return utils::json_response(utils::to_json(result));
        }

        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_create(obelisk::http::http_request_wrapper&request)
        {
            using namespace obelisk::http::validator;
            co_await request.validate({
                {"parent_id", {required(), integer(false)}},
                {"name", {required()}},
            });
            std::uint64_t parent_id = request.params()["parent_id"].get<std::uint64_t>();
            auto check_query = obelisk::database::db::select({"id"}).from({"sys_organizations"}).where({
                {col("id"), parent_id},
                {col("deleted_at"), nullptr}
            });
            if (co_await check_query.count() == 0)
                throw obelisk::http::http_exception("server.error.target_not_exists", obelisk::http::EST_UNPROCESSABLE_CONTENT);

            auto& params = request.params();
            std::vector<std::pair<std::string, sql_value>> values{{"name", request.params()["name"].get<std::string>()}};
            try_emplace<std::uint64_t>("parent_id", values, params);
            try_emplace<std::string>("director_name", values, params);
            try_emplace<std::string>("director_phone", values, params);
            try_emplace<std::string>("emergency_name", values, params);
            try_emplace<std::string>("emergency_phone", values, params);
            try_emplace<std::string>("address", values, params);

            co_await obelisk::database::db::transaction([&values, parent_id](auto connection)->boost::asio::awaitable<void>
            {
                auto prefix = obelisk::http::config::get<std::string>("database.default.prefix", "");
                auto result = co_await obelisk::database::db::insert("sys_organizations").values(values).get(connection);
                auto org_id = result.last_insert_id();
                std::string sql = std::format("INSERT INTO {2}sys_organization_hierarchy (fn_ancestor_id, fn_descendant_id, path_length) SELECT fn_ancestor_id, {0}, path_length+1 FROM {2}sys_organization_hierarchy WHERE fn_descendant_id = {1} UNION SELECT {0}, {0}, 0;", org_id, parent_id, prefix);
                co_await connection->co_query_v(sql);
                co_return;
            });
            co_return utils::json_response(nullptr);
        }

        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_update(obelisk::http::http_request_wrapper&request)
        {
            using namespace obelisk::http::validator;
            co_await request.validate({
                {"id", {required(), integer(false)}},
            });
            std::uint64_t target_id = request.params()["id"].get<std::uint64_t>();
            auto check_query = obelisk::database::db::select({"id"}).from({"sys_organizations"}).where({
                {col("id"), target_id},
                {col("deleted_at"), nullptr}
            });
            if (co_await check_query.count() == 0)
                throw obelisk::http::http_exception("server.error.target_not_exists", obelisk::http::EST_UNPROCESSABLE_CONTENT);

            auto& params = request.params();
            std::vector<std::pair<std::string, sql_value>> values;
            try_emplace<std::string>("name", values, params);
            try_emplace<std::string>("director_name", values, params);
            try_emplace<std::string>("director_phone", values, params);
            try_emplace<std::string>("emergency_name", values, params);
            try_emplace<std::string>("emergency_phone", values, params);
            try_emplace<std::string>("address", values, params);
            co_await obelisk::database::db::update({"sys_organizations"}).values(values).where({
                {col("id"), target_id}
            }).get();
            co_return nullptr;
        }

        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_delete(obelisk::http::http_request_wrapper&request)
        {
            auto target_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_organization_id"]);
            co_await obelisk::database::db::transaction([target_id](auto connection)->boost::asio::awaitable<void>
            {
                co_await obelisk::database::db::delete_from({"sys_organization_hierarchy"}).where({
                    {col("fn_ancestor_id"), target_id}
                }).or_where({
                    {col("fn_descendant_id"), target_id}
                }).get();
                co_await obelisk::database::db::update({"sys_organizations"}).where({
                    {col("id"), target_id}
                }).set({{"deleted_at", std::chrono::system_clock::now()}}).get();
            });
            co_return nullptr;
        }


        template<typename As>
        static void try_emplace(const std::string& key, std::vector<std::pair<std::string, sql_value>>& values, nlohmann::json params)
        {
            if (params.contains(key))
                values.emplace_back(key, params[key].get<As>());
        }
    };
}

#endif //SYS_ORGANIZATION_CONTROLLER_H
