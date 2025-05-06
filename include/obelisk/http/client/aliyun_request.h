#ifndef OBELISK_HTTP_CLIENT_ALIYUN_REQUEST_H
#define OBELISK_HTTP_CLIENT_ALIYUN_REQUEST_H
#include <iostream>

#include "request.h"
#include <cryptopp/sha.h>
#include <cryptopp/hmac.h>
#include <cryptopp/hex.h>
#include <boost/algorithm/hex.hpp>
namespace obelisk::http::client
{
    class aliyun_request: public request {
    public:
        aliyun_request(const std::string& uri, const std::string& method): request(uri, method)
        {
            canonical_ = method + "\n";
            canonical_ += header_.meta_.p2_ + "\n";
            canonical_ += "\n";
            canonical_ += "\n";
            canonical_ += "\n";
            canonical_ += "UNSIGNED-PAYLOAD";

            const std::string DateKey = hmac_sha_256("aliyun_v4g95zy5DiRxXcpL7L3BAbClvjTr5HoX","20250411");
            const std::string DateRegionKey = hmac_sha_256(DateKey, "cn-hangzhou");
            const std::string DateRegionServiceKey = hmac_sha_256(DateRegionKey, "oss");
            const std::string SigningKey = hmac_sha_256(DateRegionServiceKey, "aliyun_v4_request");
            std::cout << boost::algorithm::hex(SigningKey) << std::endl;
        }

    protected:

        void before_send() override {


        }
        std::string hmac_sha_256(const std::string&key, const std::string& data) {
            CryptoPP::HMAC<CryptoPP::SHA256> hmac(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size());
            std::string result;
            CryptoPP::StringSource ss(data, true, new CryptoPP::HashFilter(hmac,new CryptoPP::StringSink(result)));
            return result;
        }
        std::string canonical_;
    };

}

#endif //OBELISK_HTTP_CLIENT_ALIYUN_REQUEST_H
