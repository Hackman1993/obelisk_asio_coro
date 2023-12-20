#ifndef OBELISK_HTTP_BLOCK_DATA_H
#define OBELISK_HTTP_BLOCK_DATA_H
#include <sahara/sahara.h>
#include <memory>
#include "http_iodata.h"
namespace obelisk::http {
    struct request_meta {
        std::string p1_;
        std::string p2_;
        std::string p3_;
        bool is_req_ = false;
    };

    struct http_header{
        request_meta meta_;
        sahara::container::unordered_smap_u<std::string> headers_;
    };

    struct http_block_data {
    public:
        std::shared_ptr<std::iostream> prepare_content(std::uint64_t size);
        http_header header_;
        std::uint64_t content_length_;
        std::shared_ptr<core::http_iodata> content_;
    };
}


#endif //OBELISK_HTTP_BLOCK_DATA_H
