#ifndef HTTP_IODATA_H
#define HTTP_IODATA_H
#include <cstdint>
#include <sstream>
#include <vector>
#include <memory>
namespace obelisk::http::core {
    class base_iodata{
    public:
        virtual ~base_iodata() = default;

        virtual bool eof() = 0;
        virtual uint64_t size() = 0;
        virtual void seekg(std::int64_t pos) = 0;
        virtual std::streamsize read(unsigned char* buffer, uint32_t length) = 0;
        virtual std::streamsize write(unsigned char* buffer, uint32_t length) = 0;
    };



    class multi_stream_iodata: public base_iodata {
    public:
        multi_stream_iodata() = default;
        void append(std::unique_ptr<base_iodata> stream);
        ~multi_stream_iodata() override;
        std::streamsize read(unsigned char* buffer, uint32_t length) override;
        uint64_t size() override;
        bool eof() override;
        void seekg(std::int64_t pos) override
        {
            if (pos == -1){
                for (auto &data: datas_)
                    data->seekg(-1);
                return;
            }
            auto remain_pos = pos;
            for (auto &data: datas_)
            {
                data->seekg(0);
                auto stream_size = static_cast<std::int64_t>(data->size());
                auto consume_size = std::min(remain_pos, stream_size);
                data->seekg(consume_size);
                remain_pos -= consume_size;
                if (remain_pos <= 0)
                    return;
            }
        }
        std::streamsize write(unsigned char* buffer, uint32_t length) override;
    private:
        std::uint32_t offset_ = 0;
        std::vector<std::unique_ptr<base_iodata>> datas_;
    };
} // obelisk::http::core

#endif //HTTP_IODATA_H
