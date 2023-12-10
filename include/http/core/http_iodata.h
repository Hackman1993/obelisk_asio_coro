//
// Created by Hackman.Lo on 2023/12/10.
//

#ifndef HTTP_IODATA_H
#define HTTP_IODATA_H
#include <cstdint>
#include <fstream>

namespace obelisk::http::core {
    class http_idata {
    public:
        virtual ~http_idata() = default;

        virtual std::streamsize read(unsigned char* buffer, uint32_t length) = 0;
        virtual uint64_t size();

        virtual bool eof() = 0;
    };

    class http_odata {
    public:
        virtual ~http_odata() = default;
        virtual std::streamsize write(unsigned char* buffer, uint32_t length) = 0;
    };


    class http_file_data: public http_idata, public http_odata {
    public:
        std::streamsize read(unsigned char* buffer, uint32_t length) override;

        uint64_t size() override;

        bool eof() override;

        std::streamsize write(unsigned char* buffer, uint32_t length) override;

    private:
        std::fstream file_;
    };
} // obelisk::http::core

#endif //HTTP_IOSDATA_H
