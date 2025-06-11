//
// Created by hackman on 6/5/25.
//

#include "cards.h"

#include "obelisk/database/db.h"
#include "obelisk/http/validator/required_validator.h"

namespace module::crown_plaza::controllers
{
    using namespace obelisk::http::validator;
    using namespace obelisk::database::builder::detail;

    struct admin_model
    {

    };

    struct member_model
    {

    };
    obelisk::task<std::unique_ptr<obelisk::http::http_response>> cards::find_by_card(obelisk::http::http_request_wrapper &request) {

        co_await request.validate({
            {"card_no", {required()}}
        });

        struct card_info
        {
            std::string cardable_type;
            std::uint64_t fn_cardable_id;
        };
        auto card_results = co_await obelisk::database::db::select({col("cardable_type"), col("fn_cardable_id")}).from({"cards"}).where({
            {col("card_no"), request.params()["card_no"].get<std::string>()},
            {col("deleted_at"), nullptr},
            {col("status"), 1}
        }).get<card_info>();
        if (card_results.rows().empty())
            co_return std::make_unique<obelisk::http::json_response>(nlohmann::json::object_t{
                {"message", ""},
                {"status_code", 0},
                {"data", nullptr}
            });
        auto card_data = card_results.rows()[0];
        std::uint64_t type = 0;
        nlohmann::json::object_t result;
        if (card_results.rows()[0].cardable_type == "member"){
            type = 1;
            struct member_info
            {
                std::uint64_t member_id;
                std::optional<std::string> name;
                std::optional<std::string> number;
                std::optional<std::string> nationality;
                std::optional<std::string> passport_no;
                std::optional<std::string> picture_path;
                std::optional<std::int64_t> gender;
                std::optional<boost::mysql::date> birthday;
                std::optional<std::string> phone_number;
            };
            auto query = obelisk::database::db::select({
                {col("id"), "member_id"},
                col("name"),
                col("number"),
                col("nationality"),
                col("passport_no"),
                col("picture_path"),
                col("gender"),
                col("birthday"),
                {col("phone"),"phone_number"}
            }).from({"members"}).where({
                {col("id"), card_results.rows()[0].fn_cardable_id},
                {col("deleted_at"), nullptr},
                {col("passport_expires_at"), ">=", std::chrono::system_clock::now()},
                {col("visa_expires_at"), ">=", std::chrono::system_clock::now()}
            });
            auto member_result = co_await query.get<member_info>();
            if (member_result.rows().empty())
                co_return std::make_unique<obelisk::http::json_response>(nlohmann::json::object_t{
                    {"message", ""},
                    {"status_code", 0},
                    {"data", nullptr}
                });;
            result = to_json(member_result.rows()[0]);
            result["type"] = type;
        }
        else if (card_results.rows()[0].cardable_type == "sys_admin"){
            type = 2;
            struct admin_info
            {
                std::uint64_t operator_id;
                std::string username;
                std::optional<std::string> real_name;
                std::optional<std::string> phone_number;
                std::optional<std::string> passport_no;
                std::optional<std::string> avatar_url;
            };
            auto query = obelisk::database::db::select({
                {col("id"), "operator_id"},
                col("username"),
                col("real_name"),
                {col("phone"),"phone_number"},
                col("passport_no"),
                col("avatar_url"),
            }).from({"sys_admins"}).where({
                {col("id"), card_results.rows()[0].fn_cardable_id},
                {col("deleted_at"), nullptr},
                {col("status"), 1},
            });
            auto admin_result = co_await query.get<admin_info>();
            if (admin_result.rows().empty())
                co_return std::make_unique<obelisk::http::json_response>(nlohmann::json::object_t{
                    {"message", ""},
                    {"status_code", 0},
                    {"data", nullptr}
                });;
            result = to_json(admin_result.rows()[0]);
            result["type"] = type;
        }
        co_return std::make_unique<obelisk::http::json_response>(nlohmann::json::object_t{
            {"message", ""},
            {"status_code", 0},
            {"data", result}
        });
    }
}
