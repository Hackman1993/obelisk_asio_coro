#ifndef HTTP_IODATA_STREAM_WRAPPER_H
#define HTTP_IODATA_STREAM_WRAPPER_H
#include "http_iodata.h"
namespace obelisk::http::core {

    class http_data_istream_wrapper: public http_iodata{
    public:
        http_data_istream_wrapper(std::unique_ptr<std::iostream> t, uint64_t length) : value_(std::move(t)), length_(length){};

        ~http_data_istream_wrapper() override;

        std::streamsize read(unsigned char* buffer, uint32_t length) override;

        uint64_t size() override;

        bool eof() override;

        std::streamsize write(unsigned char* buffer, uint32_t length) override;


    private:
        std::unique_ptr<std::iostream> value_;
        std::uint64_t length_;
    };
} // obelisk::http::core

#endif //HTTP_IODATA_STREAM_WRAPPER_H
