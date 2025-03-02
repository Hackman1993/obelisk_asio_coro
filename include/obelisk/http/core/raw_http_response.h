/***********************************************************************************************************************
 * @author Hackman Lo
 * @file raw_http_response.h
 * @description 
 * @created_at 2023-10-11
***********************************************************************************************************************/

#ifndef OBELISK_RAW_HTTP_RESPONSE_H
#define OBELISK_RAW_HTTP_RESPONSE_H
#include <string>
#include <memory>
#include <utility>
#include <mutex>
#include <atomic>
    namespace obelisk::http {

        class raw_http_response {
        public:
            raw_http_response(std::string  header, std::shared_ptr<std::istream> body);
            std::uint32_t read(unsigned char *buffer, std::uint32_t length);
            std::uint32_t consume(std::uint32_t size);
        protected:
            std::mutex mutex_;
            std::atomic_uint32_t cursor_ = 0;
            std::string header_;
            std::shared_ptr<std::istream> body_;
        };

    } // obelisk
// http

#endif //OBELISK_RAW_HTTP_RESPONSE_H
