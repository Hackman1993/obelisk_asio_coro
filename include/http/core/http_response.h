#ifndef OBELISK_HTTP_RESPONSE_H
#define OBELISK_HTTP_RESPONSE_H
#include <string>
#include <memory>
#include <unordered_map>
#include "../core/http_response_code.h"
#include "raw_http_response.h"
#include "http_block_data.h"

namespace obelisk::http {

    class http_response {
    public:
        explicit http_response(EResponseCode code);

        virtual ~http_response() = default;
        virtual std::uint64_t content_length();
        virtual sahara::container::unordered_smap_u<std::string>& headers();
        virtual std::unique_ptr<core::http_iodata> serialize_header();
        virtual std::unique_ptr<core::http_iodata> serialize();
    protected:
        http_header header_;
        std::unique_ptr<core::http_iodata> body_;
        static std::unordered_map<EResponseCode, std::string> resp_status_map_;
    };

} // obelisk

#endif //OBELISK_HTTP_RESPONSE_H
