#include "auth_controller.h"

#include <sahara/hash/bcrypt.h>
#include "obelisk/obelisk.h"
#include "obelisk/database/mysql/mysql_connection.h"
#include "obelisk/http/validator/validator.h"
#include "obelisk/database/database.h"
#include "obelisk/database/builder/builder.h"

namespace controller{
    using namespace obelisk::http::validator;
obelisk::task<std::unique_ptr<obelisk::http::http_response> > auth_controller::login1(obelisk::http::http_request_wrapper &request) {
    co_await request.validate({
        {"login", {required()}},
        {"password", {required()}}
    });
    using namespace obelisk::database::query;
    const auto result = co_await builder::select({"id", "username", "password"}).from({"t_sys_admins"}).where({
        {"username", request.params()["login"].get<std::string>()}
    }).get();

    if (result.rows().empty() || !sahara::hash::bcrypt::validatePassword(request.params()["password"].get<std::string>(), result.rows()[0][2].as_string()))
         co_return std::make_unique<obelisk::http::json_response>(nlohmann::json{
             {"code", 403},
             {"message", "server.error.invalid_credential"}
         });

    //
    // std::optional<std::string> exists_access_token;
    // if (!result.rows()[0][3].is_null()) exists_access_token = result.rows()[0][3].as_string();
    // std::optional<boost::mysql::datetime> expires_point;
    // if (!result.rows()[0][4].is_null()) expires_point = result.rows()[0][4].as_datetime();
    //
    // if (!exists_access_token) {
    //     exists_access_token = sahara::utils::uuid::generate();
    //     std::chrono::time_point expire_at_chrono =
    //             boost::mysql::datetime::time_point::clock::now() + std::chrono::hours{2};
    //     expires_point = boost::mysql::datetime(
    //         std::chrono::time_point_cast<boost::mysql::datetime::time_point::duration>(expire_at_chrono));
    //     std::string token_insertion_sql = format_sql(connection->format_opts().value(),
    //                                                  "INSERT INTO t_system_admin_access_token(token_id, expire_at, fn_operator_id) values({}, {}, {})",
    //                                                  exists_access_token, expires_point,
    //                                                  result.rows()[0][0].as_uint64());
    //     boost::mysql::diagnostics diagonostics;
    //     constexpr auto token = boost::asio::as_tuple(obelisk::use_token);
    //     auto [ec] = co_await connection->async_execute(token_insertion_sql, result, diagonostics, token);
    //     boost::mysql::throw_on_error(ec, diagonostics);
    // }
    // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
    //     {"access_token", exists_access_token.value()},
    //     {
    //         "expires_at",
    //         std::format("{:%F %T}", std::chrono::zoned_time{
    //                         std::chrono::current_zone(), expires_point.value().as_time_point()
    //                     })
    //     }
    // });
    co_return nullptr;
}

obelisk::task<std::unique_ptr<obelisk::http::http_response> > auth_controller::login(
    obelisk::http::http_request_wrapper &request) {
    co_await request.validate({
        {"login", {required()}},
        {"password", {required()}}
    });

    // auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
    //
    //
    // std::string query = format_sql(connection->format_opts().value(),
    //                                              "SELECT operator_id, username, password, token_id, expire_at FROM t_operator LEFT JOIN t_system_admin_access_token ON fn_operator_id = operator_id AND expire_at > NOW() WHERE deleted_at IS NULL AND username = 'admin'",
    //                                              request.params()["login"].as_string());
    // boost::mysql::results result;
    // co_await connection->async_execute(query, result, obelisk::use_token);
    //
    // if (result.rows().empty() || !sahara::hash::bcrypt::validatePassword(
    //         request.params()["password"].as_string(), result.rows()[0][2].as_string()))
    //     co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
    //         {"code", 403},
    //         {"message", "common.auth.invalid_credential"}
    //     });
    //
    // std::optional<std::string> exists_access_token;
    // if (!result.rows()[0][3].is_null()) exists_access_token = result.rows()[0][3].as_string();
    // std::optional<boost::mysql::datetime> expires_point;
    // if (!result.rows()[0][4].is_null()) expires_point = result.rows()[0][4].as_datetime();
    //
    // if (!exists_access_token) {
    //     exists_access_token = sahara::utils::uuid::generate();
    //     std::chrono::time_point expire_at_chrono =
    //             boost::mysql::datetime::time_point::clock::now() + std::chrono::hours{2};
    //     expires_point = boost::mysql::datetime(
    //         std::chrono::time_point_cast<boost::mysql::datetime::time_point::duration>(expire_at_chrono));
    //     std::string token_insertion_sql = format_sql(connection->format_opts().value(),
    //                                                  "INSERT INTO t_system_admin_access_token(token_id, expire_at, fn_operator_id) values({}, {}, {})",
    //                                                  exists_access_token, expires_point,
    //                                                  result.rows()[0][0].as_uint64());
    //     boost::mysql::diagnostics diagonostics;
    //     constexpr auto token = boost::asio::as_tuple(obelisk::use_token);
    //     auto [ec] = co_await connection->async_execute(token_insertion_sql, result, diagonostics, token);
    //     boost::mysql::throw_on_error(ec, diagonostics);
    // }
    // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
    //     {"access_token", exists_access_token.value()},
    //     {
    //         "expires_at",
    //         std::format("{:%F %T}", std::chrono::zoned_time{
    //                         std::chrono::current_zone(), expires_point.value().as_time_point()
    //                     })
    //     }
    // });
    co_return nullptr;
}

obelisk::task<std::unique_ptr<obelisk::http::http_response> > auth_controller::logout(
    obelisk::http::http_request_wrapper &request) {
    co_return nullptr;
}

obelisk::task<std::unique_ptr<obelisk::http::http_response> > auth_controller::check_auth(
    obelisk::http::http_request_wrapper &request) {
    co_return std::make_unique<obelisk::http::json_response>(nlohmann::json{
        {"validate", true},
    });
}

obelisk::task<std::unique_ptr<obelisk::http::http_response> > auth_controller::get_permissions(
    obelisk::http::http_request_wrapper &request) {
    auto conn = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
    std::string permission_sql = boost::mysql::format_sql(conn->format_opts().value(),
                                                          R"(select t_permission.code from t_permission
                                                       left join t_mid_role_permission on t_mid_role_permission.mid_permission_id = t_permission.permission_id
                                                       left join t_mid_operator_role on t_mid_operator_role.mid_role_id = t_mid_role_permission.mid_role_id
                                                       where t_mid_operator_role.mid_operator_id={})",
                                                          std::any_cast<uint64_t>(
                                                              request.additional_data()["__operator_id"]));
    boost::mysql::results result;
    boost::mysql::diagnostics diagnostics;
    conn->set_meta_mode(boost::mysql::metadata_mode::full);
    auto [ec] = co_await conn->async_execute(permission_sql, result, diagnostics,
                                             boost::asio::as_tuple(obelisk::use_token));
    boost::mysql::throw_on_error(ec, diagnostics);
    co_return json_response(result);
}

}
