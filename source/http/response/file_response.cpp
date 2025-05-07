/***********************************************************************************************************************
 * @author Hackman Lo
 * @file file_response.cpp
 * @description 
 * @created_at 2023-10-10
***********************************************************************************************************************/

#include "obelisk/http/core/mime_types.h"
#include "obelisk/http/response/file_response.h"
#include "obelisk/http/exception/http_exception.h"
#include "obelisk/http/core/http_iodata_stream_wrapper.h"
#include <fstream>
#include <filesystem>
namespace obelisk::http {
    file_response::file_response(const std::filesystem::path &path, EResponseCode code) : http_response(code) {
        auto fs = std::make_unique<std::fstream>();
        fs->open(path, std::ios::in | std::ios::binary);
        if(fs->fail()){
            THROW(http_exception, "Permission Denied", "Obelisk", EResponseCode::EST_FORBIDDEN);
        }
        const std::string extension = path.extension().string();
        header_.headers_["Content-Type"] = mime_types::lookup(extension);

        body_ = std::make_unique<core::http_data_istream_wrapper>(std::move(fs),file_size(path));
    }
} // obelisk