//
// Created by hackman on 5/30/25.
//

#include "member_controller.h"

#include "obelisk/obelisk.h"
#include <boost/asio/awaitable.hpp>
#include <obelisk/database/database.h>
#include <obelisk/http/validator/integer_validator.h>
#include <obelisk/http/validator/required_validator.h>
#include <obelisk/http/validator/exists_validator.h>
#include "obelisk/filesystem/filesystem.h"
#include "obelisk/http/validator/file_validator.h"
#include "module/default/controllers/auth_controller.h"
#include "obelisk/http/validator/confirmed_validator.h"
#include "obelisk/http/validator/in_validator.h"

namespace module::crown_plaza::controllers
{

    using namespace obelisk::http::validator;
    using namespace obelisk::database::builder::detail;
    awaitable<std::unique_ptr<http_response>> member_controller::backend_view(http_request_wrapper&request) {

        auto query = obelisk::database::db::select({
            {col("m.id"), "id"},
            {col("m.name"), "name"},
            {col("m.number"), "number"},
            {col("m.phone"), "phone"},
            {col("m.picture_path"), "picture_path"},
            {col("m.gender"), "gender"},
            {col("m.status"), "status"},
            {col("m.birthday"), "birthday"},
            {col("m.passport_no"), "passport_no"},
            {col("m.nationality"), "nationality"},
            {col("m.visa_issue_at"), "visa_issue_at"},
            {col("m.visa_expires_at"), "visa_expires_at"},
            {col("m.passport_issue_at"), "passport_issue_at"},
            {col("m.passport_expires_at"), "passport_expires_at"},
            {col("m.passport_sign_location"), "passport_sign_location"},
            {col("m.passport_picture"), "passport_picture"},
            {col("m.last_entry_at"), "last_entry_at"}
        }).from({{"members", "m"}})
        .global_where({{col("m.deleted_at"), nullptr}}).order_by({{"m.updated_at"}, "DESC"});
        if(request.params().contains("search")) {
            auto search = request.params()["search"].get<std::string>()+"%";
            query.or_where({{col("m.name"), "LIKE", search}});
            query.or_where({{col("m.number"), "LIKE", search}});
            query.or_where({{col("m.passport_no"), "LIKE", search}});
        }

        struct member_model
        {
            std::uint64_t id{};
            std::optional<std::string> name;
            std::optional<std::string> number;
            std::optional<std::string> phone;
            std::optional<std::string> picture_path;
            std::int64_t gender{};
            std::int64_t status{};
            std::optional<boost::mysql::date> birthday;

            std::optional<std::string> passport_no;
            std::optional<std::string> nationality;
            std::optional<boost::mysql::date> visa_issue_at;
            std::optional<boost::mysql::date> visa_expires_at;
            std::optional<boost::mysql::date> passport_issue_at;
            std::optional<boost::mysql::date> passport_expires_at;
            std::optional<std::string> passport_sign_location;
            std::optional<std::string> passport_picture;
            std::optional<boost::mysql::datetime> last_entry_at;
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
            {"picture_path", {required(), file({"image/png", "image/jpeg"})}},
            {"passport_picture", {required(), file({"image/png", "image/jpeg"})}},
            {"visa_issue_at", {required()}},
            {"visa_expires_at", {required()}},
            {"passport_no", {required()}},
            {"passport_sign_location", {required()}},
            {"passport_issue_at", {required()}},
            {"passport_expires_at", {required()}}
        });
        std::vector<std::pair<col, sql_value>> values;
        try_emplace<std::string>("name", values, request.params());
        try_emplace<std::string>("number", values, request.params());
        try_emplace<std::string>("nationality", values, request.params());
        try_emplace<std::string>("birthday", values, request.params());
        try_emplace<std::uint64_t>("gender", values, request.params());
        try_emplace<std::string>("visa_issue_at", values, request.params());
        try_emplace<std::string>("visa_expires_at", values, request.params());
        try_emplace<std::string>("passport_no", values, request.params());
        try_emplace<std::string>("passport_sign_location", values, request.params());
        try_emplace<std::string>("passport_issue_at", values, request.params());
        try_emplace<std::string>("passport_expires_at", values, request.params());
        if (request.filebag().contains("picture_path")){
            auto picture_path = co_await obelisk::filesystem::guard("local")->save_random_name("/uploads/", *request.filebag()["picture_path"]);
            values.emplace_back("picture_path", picture_path);
        }
        if (request.filebag().contains("passport_picture"))
        {
            auto passport_picture = co_await obelisk::filesystem::guard("local")->save_random_name("/uploads/", *request.filebag()["passport_picture"]);
            values.emplace_back("passport_picture", passport_picture);
        }

        co_await obelisk::database::db::insert("members").values(values).get();
        co_return json_response(nullptr);
    }

    obelisk::task<std::unique_ptr<http_response>> member_controller::backend_update(http_request_wrapper &request) {
        co_await request.validate({
            {"id", {required(), integer(false), exists("members")}},
            {"birthday", {}},
            {"gender", {integer(false)}},
            {"picture_path", {file({"image/png", "image/jpeg"})}},
            {"passport_picture", {file({"image/png", "image/jpeg"})}},
            {"visa_issue_at", {}},
            {"visa_expires_at", {}},
            {"passport_issue_at", {}},
            {"passport_expires_at", {}}
        });
        std::vector<std::pair<col, sql_value>> values;
        try_emplace<std::string>("name", values, request.params());
        try_emplace<std::string>("number", values, request.params());
        try_emplace<std::string>("nationality", values, request.params());
        try_emplace<std::string>("birthday", values, request.params());
        try_emplace<std::uint64_t>("gender", values, request.params());
        try_emplace<std::string>("visa_issue_at", values, request.params());
        try_emplace<std::string>("visa_expires_at", values, request.params());
        try_emplace<std::string>("passport_no", values, request.params());
        try_emplace<std::string>("passport_sign_location", values, request.params());
        try_emplace<std::string>("passport_issue_at", values, request.params());
        try_emplace<std::string>("passport_expires_at", values, request.params());
        if (request.filebag().contains("picture_path")){
            auto picture_path = co_await obelisk::filesystem::guard("local")->save_random_name("/uploads/", *request.filebag()["picture_path"]);
            values.emplace_back("picture_path", picture_path);
        } else
            try_emplace<std::string>("picture_path", values, request.params());
        if (request.filebag().contains("passport_picture"))
        {
            auto passport_picture = co_await obelisk::filesystem::guard("local")->save_random_name("/uploads/", *request.filebag()["passport_picture"]);
            values.emplace_back("passport_picture", passport_picture);
        } else
            try_emplace<std::string>("picture_path", values, request.params());
        co_await obelisk::database::db::update({"members"}).set(values).where({{col("id"), request.params()["id"].get<std::uint64_t>()}}).get();
        co_return json_response(nullptr);
    }

    obelisk::task<std::unique_ptr<http_response>> member_controller::backend_delete(http_request_wrapper &request) {
        co_await request.validate({
            {"id", {required(), integer(false)}}
        });

        co_await obelisk::database::db::update({"members"}).set({
            {col("deleted_at"), std::chrono::system_clock::now()}
        }).where({{col("id"), request.params()["id"].get<std::uint64_t>()}}).get();
        co_return json_response(nullptr);

    }

    obelisk::task<std::unique_ptr<http_response>> member_controller::backend_sign_card(http_request_wrapper &request) {

        co_await request.validate({
                {"id", {required(), integer(false), exists("members")}},
            {"card_no", {required()}},
            {"number", {required()}},
            //{"password", {required(), confirmed()}},
            //{"card_type", {required(), integer(false), in<std::uint64_t>({1,2,3,4,5})}}
        });
        std::vector<std::pair<col, sql_value>> values;
        try_emplace<std::string>("card_no", values, request.params());
        std::string password_hash;
        if (request.params().contains("password"))
            password_hash = sahara::hash::bcrypt::generateHash(request.params()["password"].get<std::string>());
        else
            password_hash = sahara::hash::bcrypt::generateHash(std::string("123456"));
        values.emplace_back("password", password_hash);
        if (request.filebag().contains("card_type"))
            try_emplace<std::uint64_t>("card_type", values, request.params());
        else values.emplace_back("card_type", 1);
        values.emplace_back("cardable_type", "member");
        auto target_id = request.params()["id"].get<std::uint64_t>();
        values.emplace_back("fn_cardable_id", target_id);
        auto admin_id = std::any_cast<std::uint64_t>(request.additional_data()["_sys_admins_id"]);
        values.emplace_back("fn_creator_id", admin_id);
        values.emplace_back("fn_issuer_id", admin_id);

        std::string number = request.params()["number"].get<std::string>();
        co_await obelisk::database::db::transaction([values, target_id, number](auto connection)->awaitable<void>
        {
            co_await obelisk::database::db::update({"cards"}).set({
                {"deleted_at", std::chrono::system_clock::now()}
            }).where({
                {col("cardable_type"), "member"},
                {col("fn_cardable_id"), target_id}
            }).get(connection);
            co_await obelisk::database::db::insert("cards").values(values).get(connection);
            co_await obelisk::database::db::update({"members"}).set({
                {"number", number}
            }).where({
                {col("id"), target_id}
            }).get(connection);
        });

        co_return json_response(nullptr);
    }

    awaitable<std::unique_ptr<http_response>> member_controller::backend_update_certificate_info(http_request_wrapper& request)
    {
        co_await request.validate({
            {"id", {required(), integer(false), exists("members")}},
            {"passport_sign_location", {required()}},
            {"passport_picture", {required(), file({"image/png", "image/jpeg"})}},
            {"visa_issue_at", {required()}},
            {"visa_expires_at", {required()}},
            {"passport_issue_at", {required()}},
            {"passport_expires_at", {required()}},
            {"passport_no", {required()}}
        });
        std::vector<std::pair<col, sql_value>> values;
        try_emplace<std::string>("visa_issue_at", values, request.params());
        try_emplace<std::string>("visa_expires_at", values, request.params());
        try_emplace<std::string>("passport_no", values, request.params());
        try_emplace<std::string>("passport_sign_location", values, request.params());
        try_emplace<std::string>("passport_issue_at", values, request.params());
        try_emplace<std::string>("passport_expires_at", values, request.params());
        if (request.filebag().contains("passport_picture"))
        {
            auto passport_picture = co_await obelisk::filesystem::guard("local")->save_random_name("/uploads/", *request.filebag()["passport_picture"]);
            values.emplace_back("passport_picture", passport_picture);
        }
        co_await obelisk::database::db::update({"members"}).set(values).where({{col("id"), request.params()["id"].get<std::uint64_t>()}}).get();
        co_return json_response(nullptr);
    }

    obelisk::task<std::unique_ptr<obelisk::http::http_response>> member_controller::backend_freeze(obelisk::http::http_request_wrapper &request) {
        co_await request.validate({
                {"id", {required(), integer(false), exists("members")}},
            {"status", {required(), integer(false), in<std::uint64_t>({0,1})}}
        });
        co_await obelisk::database::db::update({"members"}).set({
            {"status", request.params()["status"].get<std::uint64_t>()},
        }).where({
            {col("id"), request.params()["id"].get<std::uint64_t>()},
        }).get<void>();
        co_return json_response(nullptr);
    }




    obelisk::task<std::unique_ptr<http_response>> member_controller::entrance(http_request_wrapper &request) {
        co_await request.validate({
            {"card_no", {required(), exists("cards", "card_no")}},
            {"occur_at", {required()}},
            {"type", {integer(false),in<std::uint64_t>({0, 1})}}
        });

        struct member_id_model{ std::uint64_t id; };
        auto result = co_await obelisk::database::db::select({{col("m.id"), "id"}}).from({
            {"members", "m"}
        }).join({"cards", "c"}, {
            {col("c.fn_cardable_id"),col("m.id")},
            {col("c.cardable_type"),"member"}
        }).where({
            {col("c.card_no"), request.params()["card_no"].get<std::string>()},
            {col("m.deleted_at"), nullptr}
        }).get<member_id_model>();
        if (result.rows().empty())
            co_return std::make_unique<obelisk::http::json_response>(nlohmann::json::object_t{
                {"message", "Member Not Found!"},
                {"status_code", 2000},
                {"data", nullptr}
            });

        auto type = request.params()["type"].get<std::uint64_t>();
        co_await obelisk::database::db::insert("member_entrance_records").ignore().values({
            {"fn_member_id", result.rows()[0].id},
            {"entrance", type},
            {"occur_at", request.params()["occur_at"].get<std::string>()}
        }).get();
        co_await obelisk::database::db::update({"members"}).set({
            {type?"last_entry_at":"last_leave_at", std::chrono::system_clock::now()}
        }).where({{col{"id"}, result.rows()[0].id}}).get();
        co_return json_response(nullptr);
    }

    awaitable<std::unique_ptr<http_response>> member_controller::region_statistics(http_request_wrapper& request)
    {
        using namespace obelisk::database::builder::detail;
        struct region_statistics_model
        {
            std::optional<std::string> nationality;
            std::int64_t count{};
        };
        auto query = obelisk::database::db::select({{count_t(raw("*")), "count"}, col("nationality")})
        .from({"members"}).where({{col("deleted_at"), nullptr}}).group_by("nationality");
        auto res = co_await query.get<region_statistics_model>();

        co_return json_response(to_json(res));
    }
}
