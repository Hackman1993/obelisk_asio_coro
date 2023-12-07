//
// Created by hackman on 10/8/23.
//

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
        virtual void add_header(const std::string& name, const std::string& value);
        virtual std::string serialize_header();
        [[nodiscard]] std::shared_ptr<std::istream> content() const;
        virtual std::uint64_t content_length();
        virtual bool has_header(const std::string& header);

        std::shared_ptr<obelisk::http::raw_http_response> serialize();
    protected:
        static std::unordered_map<EResponseCode, std::string> resp_status_map_;
        http_block_data data_{};
    };

} // obelisk

#endif //OBELISK_HTTP_RESPONSE_H
