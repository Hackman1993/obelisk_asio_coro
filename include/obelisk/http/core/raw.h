#ifndef OBELISK_HTTP_BLOCK_DATA_H
#define OBELISK_HTTP_BLOCK_DATA_H
#include <sahara/sahara.h>
#include <utility>
#include "io_data.h"

namespace obelisk::http {
    namespace core::raw {
        struct http_meta_raw {
            std::string p1_;
            std::string p2_;
            std::string p3_;
        };

        struct http_header_raw {
            http_header_raw(http_meta_raw meta, std::vector<std::pair<std::string, std::string> > headers): meta_(std::move(meta)) {
                for (auto &header: headers) {
                    headers_.emplace(header);
                }
            }
            http_header_raw() = default;
            http_meta_raw meta_;
            sahara::container::unordered_smap_u<std::string> headers_;
        };

        struct http_request_raw
        {
            http_header_raw header_;
            std::unique_ptr<base_iodata> body_;
        };
    }

    struct request_meta {
        std::string p1_;
        std::string p2_;
        std::string p3_;
        bool is_req_ = false;
    };


    struct http_header {
        request_meta meta_;
        sahara::container::unordered_smap_u<std::string> headers_;
    };

    // struct http_block_data {
    // public:
    //     std::shared_ptr<std::iostream> prepare_content(std::uint64_t size);
    //
    //     http_header header_;
    //     std::uint64_t content_length_;
    //     std::shared_ptr<core::http_iodata> content_;
    // };
    //
    struct url_parts {
        std::string protocol;
        std::string host;
        std::string path;
        std::string params;
    };
}


#endif //OBELISK_HTTP_BLOCK_DATA_H
