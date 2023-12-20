#include "http/core/http_iodata_stream_wrapper.h"

namespace obelisk::http::core {
    http_data_istream_wrapper::~http_data_istream_wrapper() {
    }

    std::streamsize http_data_istream_wrapper::read(unsigned char* buffer, uint32_t length) {
        value_->read(reinterpret_cast<char*>(buffer), length);
        return value_->gcount();
    }

    uint64_t http_data_istream_wrapper::size() {
        return length_;
    }

    bool http_data_istream_wrapper::eof() {
        return value_->eof();
    }

    std::streamsize http_data_istream_wrapper::write(unsigned char* buffer, uint32_t length) {
        value_->write(reinterpret_cast<char*>(buffer), length);
        return length;
    }

} // obelisk::http::core