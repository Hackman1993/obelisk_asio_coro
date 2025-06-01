//
// Created by hackman on 5/30/25.
//

#include "member_controller.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "obelisk/obelisk.h"
#include <boost/asio/awaitable.hpp>
#include "obelisk/database/mysql/mysql_connection.h"
#include <obelisk/database/database.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/integer_validator.h>
#include <obelisk/http/validator/required_validator.h>

namespace module::crown_plaza::controllers
{

    using namespace obelisk::http::validator;
    using namespace obelisk::database::builder::detail;
    awaitable<std::unique_ptr<http_response>> member_controller::backend_view(http_request_wrapper&request) {

        auto query = obelisk::database::db::select({
            {col("m.member_id"), "member_id"},
            {col("m.name"), "name"},
            {col("m.number"), "number"},
            {col("m.phone_number"), "phone_number"},
            {col("m.picture_path"), "picture_path"},
            {col("m.gender"), "gender"},
            {col("m.birthday"), "birthday"},
            {col("c.card_no"), "card_no"},
            {col("m.status"), "status"},
            {col("m.passport_no"), "passport_no"},
            {col("m.nationality"), "nationality"},
            {col("m.visa_issue_at"), "visa_issue_at"},
            {col("m.visa_expires_at"), "visa_expires_at"},
            {col("m.passport_issue_at"), "passport_issue_at"},
            {col("m.passport_expires_at"), "passport_expires_at"},
            {col("m.passport_sign_location"), "passport_sign_location"},
            {col("m.passport_picture"), "passport_picture"}
        }).left_join({"t_card", "c"}, {
            {col("m.member_id"), col("c.cardable_id")},
            {col("c.cardable_type"), "member"},
            {col("c.deleted_at"), nullptr}
        }).from({"t_member", "m"})
        .global_where({{col("m.deleted_at"), nullptr}}).order_by({{"m.updated_at"}, "DESC"});
        if(request.params().contains("search")) {
            auto search = request.params()["search"].get<std::string>()+"%";
            query.or_where({{col("m.name"), "LIKE", search}});
            query.or_where({{col("m.passport_no"), "LIKE", search}});
            query.or_where({{col("m.card_no"), "LIKE", search}});
        }

        struct member_model
        {
            std::uint64_t member_id{};
            std::string name;
            std::string number;
            std::string phone_number;
            std::string picture_path;
            std::int64_t gender{};
            std::string birthday;
            std::string card_no;
            std::int64_t status{};
            std::string passport_no;
            std::string nationality;
            std::string visa_issue_at;
            std::string visa_expires_at;
            std::string passport_issue_at;
            std::string passport_expires_at;
            std::string passport_sign_location;
            std::string passport_picture;
        };
        co_return co_await pagination<member_model>(query, co_await pagination_uniform(query, request));
    }

    awaitable<std::unique_ptr<http_response>> member_controller::backend_create(http_request_wrapper &request) {
        co_await request.validate({
            {"name", {required()}},
            {"number", {required()}},
            {"nationality", {required()}},
            {"birthday", {required()}},
            {"gender", {required(), integer(false)}},
            {"member_pic", {required()}},
            {"visa_issue_at", {required()}},
            {"visa_expires_at", {required()}},
            {"passport_no", {required()}},
            {"passport_sign_location", {required()}},
            {"passport_issue_at", {required()}},
            {"passport_expires_at", {required()}}
        });
        co_return nullptr;
        // if(request.filebag().contains("member_pic")) {
        //     picture_path = co_await save_attachment(*request.filebag()["member_pic"], "storage/images/avatars", connection, std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"]));
        // }
        // if(request.filebag().contains("passport_picture")) {
        //     passport_pic_path = co_await save_attachment(*request.filebag()["passport_picture"], "storage/images/passports", connection, std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"]));
        // }
        //
        //
        // auto query = std::format(R"(INSERT INTO t_member(name, number, phone_number, nationality, birthday, gender, picture_path,
        //     visa_issue_at, visa_expires_at, passport_no, passport_sign_location, passport_issue_at, passport_expires_at,
        //     passport_picture, description) VALUES({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}))",
        //     escape_string(request.params()["name"].as_string()),
        //     escape_string(request.params()["number"].as_string()),
        //     escape_string(request.params()["phone_number"].as_string()),
        //     escape_string(request.params()["nationality"].as_string()),
        //     escape_string(request.params()["birthday_string"].as_string()),
        //     request.params()["gender"].as_uint64(),
        //     escape_string(picture_path),
        //     escape_string(request.params()["visa_issue_at"].as_string()),
        //     escape_string(request.params()["visa_expires_at"].as_string()),
        //     escape_string(request.params()["passport_no"].as_string()),
        //     escape_string(request.params()["passport_sign_location"].as_string()),
        //     escape_string(request.params()["passport_issue_at"].as_string()),
        //     escape_string(request.params()["passport_expires_at"].as_string()),
        //     escape_string(passport_pic_path),
        //     escape_string(request.params().contains("description")? std::string(request.params()["description"].as_string()): std::string(""))
        // );
        //
        // auto [ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(ec, diagnostics);
        //co_return json_response(results);
        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::backend_update(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"member_id", {required(), integer(false)}});
        // validators.emplace_back(validator_group{"gender", {integer(false)}});
        // co_await request.validate(validators);
        //
        // auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        //
        //
        // std::string query_sql = boost::mysql::format_sql(connection->format_opts().value(),R"(SELECT member_id, picture_path, passport_picture FROM t_member where member_id = {})", request.params()["member_id"].as_uint64());
        // auto [ec] = co_await connection->async_execute(query_sql, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(ec, diagnostics);
        // if(results.rows().empty())
        //     throw obelisk::http::http_exception("error.http.objest_not_exists", obelisk::http::EST_FORBIDDEN);
        //
        // query_sql = R"(UPDATE t_member SET updated_at = NOW())";
        // if(request.filebag().contains("member_pic")) {
        //     std::string picture_path = co_await save_attachment(*request.filebag()["member_pic"], "storage/images/avatars", connection, std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"]), results.rows()[0][1].is_null()? "":results.rows()[0][1].as_string());
        //     query_sql.append(",picture_path = " + escape_string(picture_path));
        // }
        // if(request.filebag().contains("passport_picture")) {
        //     std::string passport_pic_path = co_await save_attachment(*request.filebag()["passport_picture"], "storage/images/passports", connection, std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"]), results.rows()[0][2].is_null()? "":results.rows()[0][2].as_string());
        //     query_sql.append(",passport_picture = " + escape_string(passport_pic_path));
        // }
        // if(request.params().contains("gender")) {
        //     query_sql += boost::mysql::format_sql(connection->format_opts().value(), ",gender = {}", request.params()["gender"].as_uint64());
        // }
        // if(request.params().contains("name")) {
        //     query_sql.append(",name = " + escape_string(std::string(request.params()["name"].as_string().c_str())));
        // }
        // if(request.params().contains("number")) {
        //     query_sql.append(",number = " + escape_string(std::string(request.params()["number"].as_string().c_str())));
        // }
        // if(request.params().contains("nationality")) {
        //     query_sql.append(",nationality = " + escape_string(std::string(request.params()["nationality"].as_string().c_str())));
        // }
        // if(request.params().contains("passport_no")) {
        //     query_sql.append(",passport_no = " + escape_string(std::string(request.params()["passport_no"].as_string().c_str())));
        // }
        // if(request.params().contains("birthday")) {
        //     query_sql.append(",birthday = " + escape_string(std::string(request.params()["birthday_string"].as_string().c_str())));
        // }
        // if(request.params().contains("phone_number")) {
        //     query_sql.append(",phone_number = " + escape_string(std::string(request.params()["phone_number"].as_string().c_str())));
        // }
        // if(request.params().contains("description")) {
        //     query_sql.append(",description = " + escape_string(std::string(request.params()["description"].as_string().c_str())));
        // }
        // if(request.params().contains("visa_expires_at")) {
        //     query_sql.append(",visa_expires_at = " + escape_string(std::string(request.params()["visa_expires_at"].as_string().c_str())));
        // }
        // if(request.params().contains("visa_issue_at")) {
        //     query_sql.append(",visa_issue_at = " + escape_string(std::string(request.params()["visa_issue_at"].as_string().c_str())));
        // }
        // if(request.params().contains("passport_sign_location")) {
        //     query_sql.append(",passport_sign_location = " + escape_string(std::string(request.params()["passport_sign_location"].as_string().c_str())));
        // }
        // if(request.params().contains("passport_issue_at")) {
        //     query_sql.append(",passport_issue_at = " + escape_string(std::string(request.params()["passport_issue_at"].as_string().c_str())));
        // }
        // if(request.params().contains("passport_expires_at")) {
        //     query_sql.append(",passport_expires_at = " + escape_string(std::string(request.params()["passport_expires_at"].as_string().c_str())));
        // }
        // query_sql += boost::mysql::format_sql(connection->format_opts().value(), " where member_id = {}" , request.params()["member_id"].as_uint64());
        // auto [error_code] = co_await connection->async_execute(query_sql, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(error_code, diagnostics);
        // co_return json_response(results);
        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::backend_sign_card(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"member_id", {required(), integer(false)}});
        // validators.emplace_back(validator_group{"number", {required()}});
        // validators.emplace_back(validator_group{"card_no", {required()}});
        // co_await request.validate(validators);
        //
        //
        // auto connection =co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // std::string query_sql = boost::mysql::format_sql(connection->format_opts().value(),R"(SELECT member_id, number FROM t_member where member_id = {})", request.params()["member_id"].as_uint64());
        // auto [ec] = co_await connection->async_execute(query_sql, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(ec, diagnostics);
        // if(results.rows().empty())
        //     throw obelisk::http::http_exception("error.http.objest_not_exists", obelisk::http::EST_FORBIDDEN);
        //
        // auto [auto_commit] = co_await connection->async_execute(R"(SET AUTOCOMMIT = 0)", results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(auto_commit, diagnostics);
        // auto [transaction_error] = co_await connection->async_execute(R"(START TRANSACTION)", results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(transaction_error, diagnostics);
        //
        // query_sql = boost::mysql::format_sql(connection->format_opts().value(), R"(UPDATE t_member set updated_at = NOW(), number = {} where member_id={})",
        //     request.params()["number"].as_string(),
        //     request.params()["member_id"].as_uint64());
        // auto [update_error] = co_await connection->async_execute(query_sql, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(update_error, diagnostics);
        // query_sql = boost::mysql::format_sql(connection->format_opts().value(),
        //     R"(INSERT t_card(card_no, cardable_type, cardable_id, fn_creator_id, fn_issuer_id) values ({}, 'mph_member', {}, {}, {}))",
        //     request.params()["card_no"].as_string(), request.params()["member_id"].as_uint64(),
        //     std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"]), std::any_cast<std::uint64_t>(request.additional_data()["__operator_id"])
        //     );
        //
        // auto [insert_error] = co_await connection->async_execute(query_sql, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(insert_error, diagnostics);
        //
        // auto [commit_error] = co_await connection->async_execute(R"(COMMIT)", results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(commit_error, diagnostics);
        //
        // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{});
        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::backend_delete(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"member_id", {required(), integer(false)}});
        // co_await request.validate(validators);
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        // std::string query = boost::mysql::format_sql(connection->format_opts().value(), R"(UPDATE t_member SET deleted_at = NOW() WHERE member_id={})", request.params()["member_id"].as_uint64());
        // auto [ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        //
        // boost::mysql::throw_on_error(ec, diagnostics);
        // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{});
        co_return nullptr;

    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::find_by_precise_data(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"search", {required()}});
        // co_await request.validate(validators);
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // auto connection =co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        // connection->set_meta_mode(boost::mysql::metadata_mode::full);
        // std::string query = std::format(R"(
        //     SELECT member_id, name, number, nationality, passport_no, picture_path, gender, birthday, phone_number, description, visa_expires_at, visa_issue_at, passport_sign_location, passport_issue_at, passport_expires_at, status, card_no FROM t_member
        //     LEFT JOIN t_card on cardable_id = member_id and cardable_type = 'mph_member' where name={0} or number={0} or passport_no={0} or card_no = {0} and t_card.deleted_at is null and t_member.deleted_at is null)",
        //     escape_string(request.params()["search"].as_string())
        // );
        // auto [ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(ec, diagnostics);
        // auto array = convert_to_json_array(results);
        // if(!array.empty())
        // {
        //     co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //         {"message", ""},
        //         {"status_code", 0},
        //         {"data", array[0]}
        //     });
        // }
        // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //         {"message", ""},
        //         {"status_code", 0},
        //         {"data", nullptr}
        // });
        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::find_by_card(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"card_no", {required()}});
        // co_await request.validate(validators);
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        // connection->set_meta_mode(boost::mysql::metadata_mode::full);
        // std::string query = std::format(R"(
        //     SELECT member_id, name, number, nationality, passport_no, picture_path, gender, birthday, phone_number, description, visa_expires_at, visa_issue_at, passport_sign_location, passport_issue_at, passport_expires_at, status, card_no FROM t_member
        //     LEFT JOIN t_card on cardable_id = member_id and cardable_type = 'mph_member'
        //     where card_no = {} and status = 1 and passport_expires_at > CURDATE() and visa_expires_at > CURDATE() and t_card.deleted_at is null and t_member.deleted_at is null)",
        //     escape_string(request.params()["card_no"].as_string())
        // );
        //
        //
        // auto [member_ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(member_ec, diagnostics);
        // if (!results.rows().empty()){
        //     auto arr = convert_to_json_array(results);
        //     arr[0].as_object().emplace("type", 1);
        //     co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //             {"message", ""},
        //             {"status_code", 0},
        //             {"data", arr[0].as_object()}
        //     });
        // }
        //
        // query = std::format(R"(
        //     SELECT t_operator.operator_id, t_operator.username, t_operator.email, t_operator.email_verified_at, t_operator.real_name, t_operator.status, t_operator.phone_number, t_operator.passport_no, t_operator.fn_organization_id, t_operator.photo_path, t_operator.avatar_url, t_operator.remember_token, t_operator.created_at, t_operator.updated_at, t_operator.deleted_at, t_card.card_id, t_card.card_no, t_card.cardable_type, t_card.cardable_id, t_card.fn_creator_id, t_card.fn_issuer_id FROM t_operator
        //     LEFT JOIN t_card on cardable_id = operator_id and cardable_type = 'mph_operator'
        //     where card_no = {} and status = 1 and t_card.deleted_at is null and t_operator.deleted_at is null)",
        //     escape_string(request.params()["card_no"].as_string())
        // );
        //
        // auto [operator_ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(operator_ec, diagnostics);
        // if (!results.rows().empty()){
        //     auto arr = convert_to_json_array(results);
        //     arr[0].as_object().emplace("type", 2);
        //     co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //             {"message", ""},
        //             {"status_code", 0},
        //             {"data", arr[0].as_object()}
        //     });
        // }
        // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //         {"message", ""},
        //         {"status_code", 0},
        //         {"data", nullptr}
        // });
        co_return nullptr;
    }


    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::entrance(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"card_no", {required()}});
        // validators.emplace_back(validator_group{"occur_time", {required()}});
        // validators.emplace_back(validator_group{"type", {required(), integer(false)}});
        // co_await request.validate(validators);
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // auto connection =co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        //
        // std::string query = std::format(R"(SELECT member_id from t_member, t_card where cardable_id = member_id and cardable_type='mph_member' and t_member.deleted_at is null and t_card.deleted_at is null and t_member.status = 1 and card_no={})",
        //                                 escape_string(request.params()["card_no"].as_string())
        // );
        // auto [ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(ec, diagnostics);
        // if(results.rows().empty())
        //     co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //         {"status_code", 422},
        //         {"message", "error.invalid_card_no"},
        //         {"data", nullptr}
        //     }, obelisk::http::EST_UNPROCESSABLE_CONTENT);
        // query = std::format(R"(INSERT INTO t_member_entrance_record(occur_at, entrance, fn_member_id) values ({}, {}, {}))",
        //     escape_string(request.params()["occur_time"].as_string()),
        //     request.params()["type"].as_uint64(),
        //     results.rows()[0][0].as_uint64()
        // );
        // auto [insert_ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(insert_ec, diagnostics);
        // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //         {"status_code", 0},
        //         {"message", ""},
        //         {"data", nullptr}
        // });

        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::backend_freeze(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"member_id", {required(), integer(false)}});
        // validators.emplace_back(validator_group{"status", {required(), integer(false)}});
        // co_await request.validate(validators);
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // auto connection =co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        //
        // std::string query = std::format(R"(UPDATE t_member set updated_at=NOW(), status={} where member_id = {} and deleted_at is null)",
        //     request.params()["status"].as_uint64(),
        //     request.params()["member_id"].as_uint64()
        // );
        // auto [ec] = co_await connection->async_execute(query, results, diagnostics, boost::asio::as_tuple(obelisk::use_token));
        // boost::mysql::throw_on_error(ec, diagnostics);
        //
        // co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        //         {"data", nullptr}
        // });
        co_return nullptr;
    }
}
