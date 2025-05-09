#ifndef HTTP_IODATA_STREAM_WRAPPER_H
#define HTTP_IODATA_STREAM_WRAPPER_H
#include "io_data.h"
namespace obelisk::http::core {

    class http_data_istream_wrapper final : public base_iodata{
    public:
        http_data_istream_wrapper(std::unique_ptr<std::iostream> t, uint64_t length) : value_(std::move(t)), length_(length){};
        void seekg(std::int64_t pos) override
        {
            if (value_)
                value_->seekg(pos);
        }

        explicit http_data_istream_wrapper(std::unique_ptr<std::iostream> t) : value_(std::move(t))
        {
            if (!value_)
            {
                length_ = 0;
                return;
            }
            value_->seekg(0, std::ios_base::end);
            const auto end_pos = value_->tellg();
            length_ = end_pos;
            value_->seekg(std::ios::beg);
        }

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
