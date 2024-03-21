//
// Created by Hackman.Lo on 2024/3/1.
//
#include "../storage/storage.h"
#include "attachment_controller.h"

#include "../common/user_data.h"
#include "database/connection_pool.h"
#include "database/mongo/mongo_connection.h"
#include "http/exception/http_exception.h"
#include "http/response/json_response.h"
#include "http/validator/validator.h"
using namespace obelisk::http::validator;

boost::cobalt::task<std::string> save_attachment(const std::shared_ptr<obelisk::http::http_file>& file, bsoncxx::oid uploader) {
    const std::string uuid_filename = sahara::utils::uuid::generate();
    const std::string date = sahara::string_ext::time_format(std::chrono::system_clock::now(), "%F");
    const auto fs = std::make_shared<std::fstream>(file->temp_path_, std::ios_base::in | std::ios::binary);
    const auto extension = file->extension_.has_value() ? file->extension_.value() : "";
    const auto storage_path = "temp_files/" + date + "/" + uuid_filename + extension;
    auto result = co_await *obelisk::storage::storage_manager::get("alioss").save(storage_path, fs, {{"temp", "1"}});
    if (!result.success_)
        throw obelisk::http::http_exception(result.error_->what(), obelisk::http::EST_INTERNAL_SERVER_ERROR);

    auto conn = obelisk::database::connection_manager::get_connection<obelisk::database::mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_attachment"];
    using namespace bsoncxx::builder::basic;
    collection.insert_one(make_document(
        kvp("url", storage_path),
        kvp("reference", array{}),
        kvp("file_size", static_cast<int64_t>(std::filesystem::file_size(file->temp_path_))),
        kvp("deleted_at", bsoncxx::types::b_date(std::chrono::system_clock::now() + std::chrono::days(1))),
        kvp("uploader", uploader),
        kvp("extension", extension),
        kvp("origin_name", file->filename_)
    ));

    co_return result.result_;

}
boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> upload(obelisk::http::http_request_wrapper&request) {
    co_await request.validate({
        {"file", {required(), image()}}
    });
    const auto&attachment = request.filebag()["file"];
    auto storage_url = co_await save_attachment(attachment, bsoncxx::oid(std::any_cast<const user_data&>(request.additional_data()["__user_info"]).user_id));



    co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
                                                                 {"url", storage_url}
                                                             }, obelisk::http::EResponseCode::EST_OK);
}
