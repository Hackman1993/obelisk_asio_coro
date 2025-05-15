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
            // SELECT
            //     t.id,
            //     t.name,
            //     tp.path_length,
            //     p.id AS parent_id,
            //     p.name AS parent_name
            // FROM t_sys_organizations t
            //          JOIN t_sys_organization_hierarchy tp ON t.id = tp.fn_descendant_id
            //          LEFT JOIN t_sys_organization_hierarchy tp_parent ON t.id = tp_parent.fn_descendant_id AND tp_parent.path_length = 1
            //          LEFT JOIN t_sys_organizations p ON tp_parent.fn_ancestor_id = p.id
            // WHERE tp.fn_ancestor_id = 1
            // ORDER BY tp.path_length, t.name;
            co_return utils::json_response(nullptr);
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


        template<typename As>
        static void try_emplace(const std::string& key, std::vector<std::pair<std::string, sql_value>>& values, nlohmann::json params)
        {
            if (params.contains(key))
                values.emplace_back(key, params[key].get<As>());
        }
    };
}

#endif //SYS_ORGANIZATION_CONTROLLER_H
