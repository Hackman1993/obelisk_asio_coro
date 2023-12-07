/***********************************************************************************************************************
 * @author Hackman Lo
 * @file raw_http_response.cpp
 * @description 
 * @created_at 2023-10-11
***********************************************************************************************************************/

#include "http/core/raw_http_response.h"
#include <istream>
#include <cstring>

namespace obelisk::http {
    std::uint32_t raw_http_response::read(unsigned char *buffer, std::uint32_t length) {
        std::scoped_lock<std::mutex> lock(mutex_);
        std::uint32_t bytes_read = 0;
        if (cursor_ < header_.size()) {
            auto len = std::min<std::uint32_t>(header_.size() - cursor_, length);
            memcpy(&buffer[bytes_read], &header_.data()[cursor_], len);
            bytes_read += len;
        }

        if (bytes_read == length)
            return bytes_read;
        while (!body_->eof()) {
            auto available = length - bytes_read;
            body_->read(reinterpret_cast<char *>(&buffer[bytes_read]), available);
            auto read = body_->gcount();
            bytes_read += read;
            if (bytes_read == length)
                break;
        }
        return bytes_read;
    }

    std::uint32_t raw_http_response::consume(std::uint32_t size) {
        return cursor_ += size;
    }

    raw_http_response::raw_http_response(std::string header, std::shared_ptr<std::istream> body) : header_(
            std::move(header)), body_(std::move(body)) {
        body_->seekg(std::ios::beg);
    }
} // obelisk
// http