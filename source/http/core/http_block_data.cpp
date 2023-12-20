#include "http/core/http_block_data.h"
#include "http/core/http_request.h"

std::shared_ptr<std::iostream> obelisk::http::http_block_data::prepare_content(std::uint64_t size) {
    // content_length_ = size;
    // if (content_length_ > 1 * 1024 * 1024) {
    //     // If content length greater than 1 MB, Create a filestream
    //     std::string path = "./" + sahara::utils::uuid::generate();
    //     content_ = std::make_shared<core::http_file_data>(path);
    // } else {
    //     // If content length <= 1MB, Create a memory buffer
    //     content_ = std::make_shared<core::http_string_data>();
    // }
    // return content_;
    return nullptr;
}
