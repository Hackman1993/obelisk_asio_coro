#ifndef HTTP_IODATA_H
#define HTTP_IODATA_H
#include <cstdint>
#include <sstream>
#include <vector>
#include <memory>
namespace obelisk::http::core {
    class http_iodata{
    public:
        virtual ~http_iodata() = default;

        virtual bool eof() = 0;
        virtual uint64_t size() = 0;
        virtual std::streamsize read(unsigned char* buffer, uint32_t length) = 0;
        virtual std::streamsize write(unsigned char* buffer, uint32_t length) = 0;
    };



    class http_multi_source_iodata: public http_iodata {
    public:
        http_multi_source_iodata() = default;

        void append(std::unique_ptr<http_iodata> stream);

        ~http_multi_source_iodata() override;

        std::streamsize read(unsigned char* buffer, uint32_t length) override;

        uint64_t size() override;

        bool eof() override;

        std::streamsize write(unsigned char* buffer, uint32_t length) override;

    private:
        std::uint32_t offset_ = 0;
        std::vector<std::unique_ptr<http_iodata>> datas_;
    };
} // obelisk::http::core

#endif //HTTP_IOSDATA_H
