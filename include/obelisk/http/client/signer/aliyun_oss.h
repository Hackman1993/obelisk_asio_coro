//
// Created by hackman on 5/13/25.
//

#ifndef ALIYUN_OSS_H
#define ALIYUN_OSS_H
#include <boost/algorithm/string/case_conv.hpp>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <ranges>
#include <boost/algorithm/hex.hpp>
#include <cryptopp/filters.h>

#include "base_signer.h"
namespace obelisk::http::client::signer
{
    using namespace boost::algorithm;
    class aliyun_oss final : public base_signer{
    public:
        aliyun_oss(std::string ak_id, std::string ak_secret, std::string region, std::string bucket):bucket_(std::move(bucket)),region_(std::move(region)), ak_id_(std::move(ak_id)), ak_secret_(std::move(ak_secret))
        {
        }
        void sign_request(core::raw::http_request_raw& raw) override
        {
            std::chrono::system_clock::time_point send_time = std::chrono::system_clock::now();
            raw.header_.headers_.emplace("Date", std::format("{:%a, %d %b %Y %H:%M:%OS GMT}", send_time));
            raw.header_.headers_.emplace("x-oss-date", std::format("{:%Y%m%dT%H%M%OSZ}", send_time));
            std::string canonical_request;
            canonical_request.append(std::format("{}\n", raw.header_.meta_.p1_));
            auto deli_pos = raw.header_.meta_.p2_.find('?');
            std::string path = raw.header_.meta_.p2_.substr(0, deli_pos);
            std::string query = deli_pos==std::string::npos? std::string{}:raw.header_.meta_.p2_.substr(deli_pos+1);
            canonical_request.append(std::format("/{}{}\n", bucket_, path));
            canonical_request.append(std::format("{}\n", query));
            if (!raw.header_.headers_.contains("x-oss-content-sha256"))
                raw.header_.headers_.emplace("x-oss-content-sha256", "UNSIGNED-PAYLOAD");

            std::vector<std::string> canonical_headers;
            std::vector<std::string> additional_headers;
            for (const auto& first : raw.header_.headers_ | std::views::keys)
            {
                if (auto str = to_lower_copy(first); str=="content-md5"|| str == "content-type" || str.starts_with("x-oss-"))
                    canonical_headers.emplace_back(std::move(str));
                else
                {
                    canonical_headers.push_back(str);
                    additional_headers.emplace_back(std::move(str));
                }
            }

            std::ranges::sort(canonical_headers);
            std::ranges::sort(additional_headers);
            for (auto & canonical_header : canonical_headers)
            {
                canonical_request.append(std::format("{}:{}\n", canonical_header, raw.header_.headers_[canonical_header]));
            }
            canonical_request.append("\n");
            std::string additional_header_str;
            for (int i = 0; i< additional_headers.size(); ++i)
            {
                if (i > 0) additional_header_str.append(";");
                additional_header_str.append(additional_headers[i]);
            }
            canonical_request.append(std::format("{}\n", additional_header_str));
            canonical_request.append("UNSIGNED-PAYLOAD");
            std::string scope = std::format("{:%Y%m%d}/{}/oss/aliyun_v4_request", send_time, region_);

            std::string string_to_sign = std::format("OSS4-HMAC-SHA256\n{:%Y%m%dT%H%M%OSZ}\n{}\n{}", send_time, scope, hex_lower(sha_256_(canonical_request)));

            std::string date_key = hmac_sha_256_(std::format("aliyun_v4{}", ak_secret_), std::format("{:%Y%m%d}", send_time));
            std::string date_region_key = hmac_sha_256_(date_key, region_);
            std::string date_region_service_key = hmac_sha_256_(date_region_key, "oss");
            std::string sign_key = hmac_sha_256_(date_region_service_key, "aliyun_v4_request");
            std::string signature = hex_lower(hmac_sha_256_(sign_key, string_to_sign));

            std::string authorization_header = std::format("OSS4-HMAC-SHA256 Credential={}/{:%Y%m%d}/{}/oss/aliyun_v4_request{},Signature={}",
                ak_id_,send_time, region_, additional_header_str.empty()? "":",AdditionalHeaders="+additional_header_str,signature);
            raw.header_.headers_.emplace("Authorization", authorization_header);
        }
    protected:
        static std::string hmac_sha_256_( const std::string&key, const std::string& data) {
            CryptoPP::HMAC<CryptoPP::SHA256> hmac(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size());
            std::string result;
            CryptoPP::StringSource ss(data, true, new CryptoPP::HashFilter(hmac,new CryptoPP::StringSink(result)));
            return result;
        }

        static std::string sha_256_(const std::string& data) {
            CryptoPP::SHA256 sha256;
            sha256.Update(reinterpret_cast<const CryptoPP::byte*>(data.data()), data.length());
            std::string result;
            result.resize(CryptoPP::SHA256::DIGESTSIZE);
            sha256.Final(reinterpret_cast<CryptoPP::byte*>(result.data()));
            return result;
        }

        static std::string sha_256_(const std::unique_ptr<std::iostream>& body)
        {
            std::string result;
            CryptoPP::SHA256 sha256;
            result.resize(CryptoPP::SHA256::DIGESTSIZE);
            if (!body)
            {
                sha256.Update(reinterpret_cast<const CryptoPP::byte*>(""),0);
                sha256.Final(reinterpret_cast<CryptoPP::byte*>(result.data()));
                return result;
            }

            body->seekg(std::ios::beg);
            do
            {
                char buf[1024] = {};
                body->read(buf, 1024);
                const auto bytes_read = body->gcount();
                sha256.Update(reinterpret_cast<const CryptoPP::byte*>(buf), bytes_read);
                if (bytes_read <= 0) break;
            }while (true);


            sha256.Final(reinterpret_cast<CryptoPP::byte*>(result.data()));
            body->seekg(std::ios::beg);
            return result;
        }

        static std::string sha_256_(const std::unique_ptr<core::base_iodata>& body)
        {
            std::string result;
            CryptoPP::SHA256 sha256;
            result.resize(CryptoPP::SHA256::DIGESTSIZE);

            if (!body)
            {
                sha256.Update(reinterpret_cast<const CryptoPP::byte*>(""),0);
                sha256.Final(reinterpret_cast<CryptoPP::byte*>(result.data()));
                return result;
            }

            body->seekg(std::ios::beg);
            std::string buffer;
            buffer.resize(1024*10);
            do
            {
                const auto bytes_read = body->read(reinterpret_cast<unsigned char*>(buffer.data()), buffer.length());
                sha256.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), bytes_read);
                if (bytes_read <= 0) break;
            }while (true);
            sha256.Final(reinterpret_cast<CryptoPP::byte*>(result.data()));
            body->seekg(std::ios::beg);
            return result;
        }

        std::string bucket_;
        std::string region_;
        std::string ak_id_;
        std::string ak_secret_;
    };

}

#endif //ALIYUN_OSS_H
