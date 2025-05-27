//
// Created by Hackman.Lo on 2024/3/1.
//
#include "controller_base.h"
#include <iostream>
#include <obelisk/http/validator/integer_validator.h>

#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/response/json_response.h>
#include <sahara/log/log.h>
#include <boost/mysql.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "obelisk/database/mysql/mysql_connection.h"
#include <nlohmann/json.hpp>
std::unique_ptr<obelisk::http::http_response> controller_base::json_response(const boost::mysql::results &result,const std::unordered_map<std::string, boost::json::value>& additional_field, obelisk::http::EResponseCode code) {


    // boost::json::array data = convert_to_json_array(result);
    //
    //
    // boost::json::object resp_object{{"data", data}};
    // for(auto &item: additional_field) {
    //     resp_object.emplace(item.first, item.second);
    // }

    // TODO:// Impl
    return nullptr;
}

obelisk::task<std::string> controller_base::save_attachment(obelisk::http::http_file &file, std::string path, std::shared_ptr<mysql_connection> connection, std::uint64_t uploader_id, const std::string& replace) {
    // const auto temp_path = std::filesystem::path(file.temp_path_);
    // auto save_path = std::filesystem::path(path).append(temp_path.filename().string()).string();
    // auto absolute_save_path = std::filesystem::path(global_configuration::webroot).append(save_path);
    // boost::algorithm::replace_all(save_path, "\\", "/");
    // boost::mysql::results results;
    // boost::mysql::diagnostics diagnostics;
    // boost::mysql::error_code error_code;
    //
    // bool replace_exists = false;
    // std::uint64_t replace_attachmet_id = 0;
    //
    // if(!replace.empty()) {
    //     auto check_query = boost::mysql::format_sql(connection->format_opts().value(), R"(SELECT attachment_id, storage_path from t_attachment where storage_path = {} AND deleted_at IS NULL LIMIT 1)", replace);
    //     auto [ec] = co_await connection->async_execute(check_query, results, diagnostics, boost::asio::as_tuple(boost::cobalt::use_op));
    //     boost::mysql::throw_on_error(ec, diagnostics);
    //     if(!results.rows().empty()) {
    //         replace_exists = true;
    //         std::filesystem::remove(std::filesystem::path(global_configuration::webroot.string() + std::string(results.rows()[0][1].as_string())));
    //         replace_attachmet_id = results.rows()[0][0].as_uint64();
    //     }
    // }
    //
    // std::string query;
    // if(!replace_exists) {
    //     query = boost::mysql::format_sql(connection->format_opts().value(), R"(INSERT INTO t_attachment(storage_path, visit_path, provider, file_size, origin_name, uploader_id, persist) values({}, {}, {}, {}, {}, {}, {}))", "/" + save_path, "/" + save_path, "filesystem", std::filesystem::file_size(temp_path), file.filename_, uploader_id, 1);
    // }else {
    //     query = boost::mysql::format_sql(connection->format_opts().value(),
    //         R"(UPDATE t_attachment SET storage_path = {}, visit_path = {}, file_size = {}, origin_name = {}, updated_at = NOW(), uploader_id = {} WHERE attachment_id = {})",
    //             "/" + save_path, "/" + save_path, std::filesystem::file_size(temp_path), file.filename_, uploader_id, replace_attachmet_id
    //         );
    // }
    // auto [ec] = co_await connection->async_execute(query, results, diagnostics,boost::asio::as_tuple(boost::cobalt::use_op));
    // boost::mysql::throw_on_error(ec, diagnostics);
    // std::filesystem::create_directories(absolute_save_path.parent_path());
    // std::filesystem::rename(file.temp_path_, absolute_save_path.string());
    co_return "/";
}

std::string controller_base::escape_string(const std::string& str) {
    return std::format("'{}'",boost::algorithm::replace_all_copy(str,"'", "\\'"));
//    std::string escaped_result;
//
//    auto escape_error = boost::mysql::escape_string(str, connection->format_opts().value(), boost::mysql::quoting_context::single_quote, escaped_result);
//    if(!escape_error){
//        return escaped_result;
//    }
//    throw obelisk::http::http_exception("String Escaping Failed", obelisk::http::EST_UNPROCESSABLE_CONTENT);
}

// std::string controller_base::escape_string(const boost::json::string &str) {
//     return escape_string(std::string(str));
// }

std::string controller_base::escape_string(const std::optional<std::string> &str) {
    if(str.has_value())
        return escape_string(str.value());
    return "null";
}

obelisk::task<std::unique_ptr<obelisk::http::http_response>> controller_base::getTerminalToken(obelisk::http::http_request_wrapper &request) {

    co_return std::make_unique<obelisk::http::json_response>(nlohmann::json{
            { "terminal", nlohmann::json{
                    {"name", "Official Backend"},
                    {"terminal_id", 1},
                    {"status", 1},
                    {"description", "Official Backend,Please Don't Delete"},
                    {"token_limit", 1}
            }},
            {"access_token", sahara::utils::uuid::generate()}
    });
}

// boost::json::array controller_base::convert_to_json_array(const boost::mysql::results &result) {
//     if(result.meta().size() <= 0)
//         return boost::json::array{};
//
//     boost::json::array data;
//     std::unordered_map<uint16_t, boost::mysql::string_view> column_names;
//     if(result.meta().size()>1){
//         for(auto &column : result.meta()) {
//             column_names.emplace(column_names.size(),column.column_name());
//         }
//     }
//     auto single_column_mode = result.meta().size()<=1;
//     for(int i =0; i< result.rows().size(); ++i) {
//         boost::json::object obj;
//         for(int j=0; j<result.rows()[i].size(); ++j) {
//             auto val = result.rows()[i][j];
//             if(!single_column_mode) {
//                 if(val.is_date())
//                     obj.emplace(column_names[j],std::format("{:%F}", std::chrono::zoned_time{std::chrono::current_zone(), val.as_date().as_time_point()}));
//                 else if(val.is_datetime())
//                     obj.emplace(column_names[j],std::format("{:%F %T}", std::chrono::zoned_time{std::chrono::current_zone(), val.as_datetime().as_time_point()}));
//                 else if(val.is_time())
//                     obj.emplace(column_names[j], val.as_time().count());
//                 else if(val.is_double())
//                     obj.emplace(column_names[j],val.as_double());
//                 else if(val.is_float())
//                     obj.emplace(column_names[j], val.as_float());
//                 else if(val.is_int64())
//                     obj.emplace(column_names[j], val.as_int64());
//                 else if(val.is_uint64())
//                     obj.emplace(column_names[j], val.as_uint64());
//                 else if(val.is_null())
//                     obj.emplace(column_names[j], nullptr);
//                 else if(val.is_string())
//                     obj.emplace(column_names[j], std::string(val.as_string()));
//                 else {
//                     LOG_MODULE_WARN("UserPart", "Unsupported column type(column: {})", std::string(column_names[i]));
//                 }
//             }else {
//                 if(val.is_date())
//                     data.emplace_back(std::format("{:%F}", std::chrono::zoned_time{std::chrono::current_zone(), val.as_date().as_time_point()}));
//                 else if(val.is_datetime())
//                     data.emplace_back(std::format("{:%F %T}", std::chrono::zoned_time{std::chrono::current_zone(), val.as_datetime().as_time_point()}));
//                 else if(val.is_time())
//                     data.emplace_back( val.as_time().count());
//                 else if(val.is_double())
//                     data.emplace_back(val.as_double());
//                 else if(val.is_float())
//                     data.emplace_back( val.as_float());
//                 else if(val.is_int64())
//                     data.emplace_back( val.as_int64());
//                 else if(val.is_int64())
//                     data.emplace_back( val.as_uint64());
//                 else if(val.is_null())
//                     data.emplace_back( nullptr);
//                 else if(val.is_string())
//                     data.emplace_back( std::string(val.as_string()));
//                 else {
//                     LOG_MODULE_WARN("UserPart", "Unsupported column type(column: {})", std::string(column_names[i]));
//                 }
//             }
//
//         }
//         if(!single_column_mode)
//             data.emplace_back(obj);
//     }
//     return data;
// }

