//
// Created by Hackman.Lo on 11/3/2023.
//


#include "http/core/http_block_data.h"
#include "http/core/http_request.h"

std::shared_ptr<std::iostream> &obelisk::http::http_block_data::prepare_content(std::uint64_t size) {
    content_length_ = size;
    if (content_length_ > 1 * 1024 * 1024) {
        // If content length greater than 1 MB, Create a filestream
        std::string path = "./" + sahara::utils::uuid::generate();
        content_ = std::make_shared<http_temp_fstream>(path);
    } else {
        // If content length <= 1MB, Create a memory buffer
        content_ = std::make_shared<std::stringstream>();
    }
    return content_;
}
