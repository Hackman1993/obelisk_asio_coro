#include "http/core/http_iodata.h"

namespace obelisk::http::core {

    void http_multi_source_iodata::append(std::unique_ptr<http_iodata> stream) {
        datas_.push_back(std::move(stream));
    }

    http_multi_source_iodata::~http_multi_source_iodata() {
    }

    std::streamsize http_multi_source_iodata::read(unsigned char* buffer, uint32_t length) {
        if(offset_ >= datas_.size()) return 0;
        std::streamsize total_read = 0;
        do {
            const uint32_t bytes_to_read = length-total_read;
            total_read += datas_[offset_]->read(&buffer[total_read], bytes_to_read);
            if(datas_[offset_]->eof()) offset_ ++;
        }while(offset_ < datas_.size() && total_read < length);
        return total_read;
    }

    uint64_t http_multi_source_iodata::size() {
        std::uint64_t ret = 0;
        for(auto &s : datas_) {
            ret += s->size();
        }
        return ret;
    }

    bool http_multi_source_iodata::eof() {
        return offset_ >= datas_.size();
    }
std::streamsize http_multi_source_iodata::write(unsigned char* buffer, uint32_t length) {
        return 0;
}
} // obelisk