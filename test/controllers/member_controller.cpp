#include "member_controller.h"

#include <boost/algorithm/string.hpp>

#include "obelisk/obelisk.h"
#include <boost/asio/awaitable.hpp>
#include "obelisk/database/mysql/mysql_connection.h"
#include <obelisk/database/database.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/integer_validator.h>
#include <obelisk/http/validator/required_validator.h>

#include "common/global_configuration.h"
namespace controller{
    using namespace obelisk::http::validator;
    obelisk::task<std::unique_ptr<http_response>> member_controller::view(http_request_wrapper&request) {
        co_await request.validate({
            {"page", {integer(), integer()}},
            {"limit", {integer()}}
        });

        auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        connection->set_meta_mode(boost::mysql::metadata_mode::full);
        std::uint32_t page = 1;
        std::uint32_t limit = 10;
        std::string search;
        std::uint64_t total = 0;

        if(request.params().contains("page")) {
            auto val = request.params()["page"].get<std::uint64_t>();
            page = val > 0? val : page;
        }
        if(request.params().contains("limit")) {
            auto val = request.params()["limit"].get<std::uint64_t>();
            limit = val > 0? val : limit;
        }
        if(request.params().contains("search")) {
            search = request.params()["search"].get<std::string>();
            if(!search.empty() && search[search.size()-1] == '\0')
                search.resize(search.size()-1);
        }

        boost::mysql::format_context ctx(connection->format_opts().value());

        ctx.append_raw(R"(SELECT member_id,name, number, phone_number, picture_path, gender, birthday, b.card_no, status)");
        ctx.append_raw(R"(, passport_no, nationality, a.visa_issue_at, a.visa_expires_at, a.passport_issue_at, a.passport_expires_at, a.passport_sign_location, a.passport_picture )");
        ctx.append_raw(R"( from t_member a
        left join t_card b on a.member_id = b.cardable_id and cardable_type = 'mph_member' and b.deleted_at is null
        where a.deleted_at is null )");

        // Search Filter
        if(!search.empty())
            boost::mysql::format_sql_to(ctx, "and (name like {0} or number like {0} or passport_no like {0} or card_no like {0}) ", search+"%");

        ctx.append_raw("order by a.updated_at desc ");

        auto query_sql = std::move(ctx).get().value();
        auto count_sql = std::format("select count(1) from ({}) as t", query_sql);

        boost::mysql::results result;
        boost::mysql::diagnostics diagnostics;
        auto [ec] = co_await connection->async_execute(count_sql, result, boost::asio::as_tuple(obelisk::use_token));
        boost::mysql::throw_on_error(ec, diagnostics);

        if(result.rows()[0][0].as_int64() == 0) {
            co_return std::make_unique<obelisk::http::json_response>(nlohmann::json{
                { "total", 0 },
                { "page", 1 },
                { "limit", limit},
                { "data", nlohmann::json{}}
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::create(obelisk::http::http_request_wrapper &request) {
        // std::vector<validator_group> validators;
        // validators.emplace_back(validator_group{"name", {required()}});
        // validators.emplace_back(validator_group{"number", {required()}});
        // validators.emplace_back(validator_group{"nationality", {required()}});
        // validators.emplace_back(validator_group{"birthday", {required()}});
        // validators.emplace_back(validator_group{"gender", {required(), integer(false)}});
        // validators.emplace_back(validator_group{"member_pic", {required()}});
        // validators.emplace_back(validator_group{"visa_issue_at", {required()}});
        // validators.emplace_back(validator_group{"visa_expires_at", {required()}});
        // validators.emplace_back(validator_group{"passport_no", {required()}});
        // validators.emplace_back(validator_group{"passport_sign_location", {required()}});
        // validators.emplace_back(validator_group{"passport_issue_at", {required()}});
        // validators.emplace_back(validator_group{"passport_expires_at", {required()}});
        // co_await request.validate(validators);
        //
        // auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("mysql");
        //
        // boost::mysql::results results;
        // boost::mysql::diagnostics diagnostics;
        // std::optional<std::string> picture_path;
        // std::optional<std::string> passport_pic_path;
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::update(obelisk::http::http_request_wrapper &request) {
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::sign_card(obelisk::http::http_request_wrapper &request) {
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::soft_delete(obelisk::http::http_request_wrapper &request) {
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::findByPreciseData(obelisk::http::http_request_wrapper &request) {
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::getCardOwner(obelisk::http::http_request_wrapper &request) {
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

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::freeze(obelisk::http::http_request_wrapper &request) {
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