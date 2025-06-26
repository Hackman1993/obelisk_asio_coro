#ifndef OBELISK_HTTP_RESPONSE_H
#define OBELISK_HTTP_RESPONSE_H
#include <string>
#include <memory>
#include <unordered_map>
#include "../core/http_response_code.h"
#include "raw_http_response.h"
#include "raw.h"

namespace obelisk::http {

    class http_response {
    public:
        explicit http_response(EResponseCode code);
        explicit http_response(core::raw::http_header_raw header, std::unique_ptr<core::base_iodata> body = nullptr): header_(std::move(header)), body_(std::move(body)) {};

        virtual ~http_response() = default;
        virtual std::uint64_t content_length();
        virtual sahara::container::unordered_smap_u<std::string>& headers();
        virtual std::unique_ptr<core::base_iodata> serialize_header();
        virtual std::unique_ptr<core::base_iodata> serialize();
        const core::raw::http_header_raw& header_raw(){return header_;}
        const std::unique_ptr<core::base_iodata>& response_body()
        {
            return body_;
        }
    protected:
        core::raw::http_header_raw header_;
        std::unique_ptr<core::base_iodata> body_;
        static std::unordered_map<EResponseCode, std::string> resp_status_map_;
    };

} // obelisk

#endif //OBELISK_HTTP_RESPONSE_H
