//
// Created by Hackman.Lo on 2024/3/1.
//
#include "../storage/storage.h"
#include "attachment_controller.h"

#include "http/exception/http_exception.h"
#include "http/response/json_response.h"
#include "http/validator/validator.h"
using namespace obelisk::http::validator;

boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> upload(obelisk::http::http_request_wrapper&request) {
    co_await request.validate({
        {"file", {required(), image()}}
    });

    const std::string uuid_filename = sahara::utils::uuid::generate();
    const std::string date = sahara::string_ext::time_format(std::chrono::system_clock::now(), "%F");

    const auto& attachment = request.filebag()["file"];
    const auto fs = std::make_shared<std::fstream>(attachment->temp_path_, std::ios_base::in | std::ios::binary);
    const auto extension = attachment->extension_.has_value()? attachment->extension_.value() : "";
    const auto storage_path = "temp_files/" + date + "/" + uuid_filename + extension;
    auto result = co_await *obelisk::storage::storage_manager::get("alioss")
            .save(storage_path, fs, {{"temp", "1"}});

    if(!result.success_)
        throw obelisk::http::http_exception(result.error_->what(), obelisk::http::EST_INTERNAL_SERVER_ERROR);


    co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
                                                                 {"url", result.result_}
                                                             }, obelisk::http::EResponseCode::EST_OK);
}