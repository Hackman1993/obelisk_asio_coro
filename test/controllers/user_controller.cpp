#include "auth_controller.h"

#include "user_controller.h"

#include <boost/algorithm/string.hpp>

#include "obelisk/obelisk.h"
#include <boost/asio/awaitable.hpp>
#include "database/mysql/mysql_connection.h"
#include <obelisk/database/connection_pool.h>
#include <obelisk/http/validator/integer_validator.h>

obelisk::task<std::unique_ptr<obelisk::http::http_response>> user_controller::view(obelisk::http::http_request_wrapper&request) {
    std::vector<validator_group> validators;
    validators.push_back(validator_group{"page", {integer()}});
    validators.push_back(validator_group{"limit", {integer()}});
    co_await request.validate(validators);

    auto connection = obelisk::database::connection_manager::get_connection<mysql_connection>("mysql");
    connection->set_meta_mode(boost::mysql::metadata_mode::full);
    std::uint32_t page = 1;
    std::uint32_t limit = 10;
    std::string search;
    std::uint64_t total = 0;

    if(request.params().contains("page")) {
        auto val = request.params()["page"].as_int64();
        page = val > 0? val : page;
    }
    if(request.params().contains("limit")) {
        auto val = request.params()["limit"].as_int64();
        limit = val > 0? val : limit;
    }
    if(request.params().contains("search")) {
        boost::trim(request.params()["search"].as_string());
        search = request.params()["search"].as_string();
        if(!search.empty() && search[search.size()-1] == '\0')
            search.resize(search.size()-1);
    }

    boost::mysql::format_context ctx(connection->format_opts().value());
    ctx.append_raw(R"(SELECT operator_id, username, real_name, phone_number, t_operator.status, passport_no, avatar_url, t_operator.created_at, t_card.card_no, t_organization.name as organization_name from t_operator
        left join t_organization on t_operator.fn_organization_id = t_organization.organization_id and t_organization.status > 0 and t_organization.deleted_at is null
        left join t_card on t_operator.operator_id = t_card.cardable_id and cardable_type = 'mph_operator' and t_card.deleted_at is null where t_operator.deleted_at is null )");

    // Organization Filter
    if(std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"]) != 1)
        boost::mysql::format_sql_to(ctx, "and fn_organization_id", std::any_cast<std::uint64_t>(request.additional_data()["__operator_organization_id"]));

    // Search Filter
    if(!search.empty())
        boost::mysql::format_sql_to(ctx, "and (username like {0} or real_name like {0} or card_no like {0} or phone_number like {0}) ", search+"%");

    ctx.append_raw("order by t_operator.updated_at desc ");

    auto query_sql = std::move(ctx).get().value();
    auto count_sql = std::format("select count(1) from ({}) as t", query_sql);

    boost::mysql::results result;
    boost::mysql::diagnostics diagnostics;
    auto [ec] = co_await connection->async_execute(count_sql, result, boost::asio::as_tuple(obelisk::use_token));
    boost::mysql::throw_on_error(ec, diagnostics);

    if(result.rows()[0][0].as_int64() == 0) {
        co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
            { "total", 0 },
            { "page", 1 },
            { "limit", limit},
            { "data", boost::json::array{}}
        });
    }
    total = result.rows()[0][0].as_int64();
    auto max_page = total/limit + (total%limit>0? 1:0);
    page = std::clamp<uint64_t>(page, 1, max_page);
    query_sql.append(boost::mysql::format_sql(connection->format_opts().value()," limit {} offset {}", limit, limit * (page -1)) );

    auto [error_code] = co_await connection->async_execute(query_sql, result, boost::asio::as_tuple(obelisk::use_token));
    boost::mysql::throw_on_error(error_code, diagnostics);

    co_return json_response(result, {
        { "total", total},
        { "page", page},
        { "limit", limit}
    });
}


