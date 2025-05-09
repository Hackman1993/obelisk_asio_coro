//
// Created by hackman on 5/9/25.
//

#ifndef ALIYUN_CLIENT_H
#define ALIYUN_CLIENT_H
#include <obelisk/http/core/base_client.h>
#include <cryptopp/sha.h>
#include <cryptopp/hmac.h>
#include <cryptopp/hex.h>
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include <expected>
namespace obelisk::http::client
{
    using namespace boost::asio;
    class aliyun_client : core::base_client{
    public:
        aliyun_client(std::string ak_id, std::string ak_secret, std::string region, std::string endpoint):
            access_key_id_(std::move(ak_id)), access_key_secret_(std::move(ak_secret)), region_(std::move(region)), endpoint_(std::move(endpoint))
        {
        }

        const std::string& region() const
        {
            return region_;
        }
        void before_send_(core::raw::http_request_raw& raw) override
        {
            std::string signed_headers;
            auto canonical_request = canonical_request_(raw, signed_headers);
            std::string string_to_sign = string_to_sign_(canonical_request);
            std::string authorization_value = std::format("ACS3-HMAC-SHA256 Credential={},SignedHeaders={},Signature={}", access_key_id_, signed_headers, boost::algorithm::hex_lower(hmac_sha_256_(access_key_secret_, string_to_sign)));

            std::cout << canonical_request << std::endl;
            raw.header_.headers_.emplace("Authorization", authorization_value);


        }
        awaitable<std::unique_ptr<http_response>> send_request(const std::string& url, const std::string&method, std::unordered_map<std::string, std::string> headers, std::unique_ptr<std::iostream> body, std::string version,std::string action)
        {
            headers.emplace("x-acs-version", version);
            headers.emplace("x-acs-action", action);
            headers.emplace("x-acs-content-sha256", boost::algorithm::hex_lower(sha_256_(body)));
            std::chrono::system_clock::time_point send_time = std::chrono::system_clock::now();
            headers.emplace("x-acs-date", std::format("{:%Y%m%dT%H%M%SZ}", send_time));
            //raw.headers_.emplace("x-acs-date", std::format("{:%Y%m%dT%H%M%SZ}", send_time));
            auto nonce = sahara::utils::uuid::generate();
            boost::replace_all(nonce,"-", "");
            headers.emplace("x-acs-signature-nonce", nonce);
            co_return co_await base_client::send_request(url, method, headers, body? std::make_unique<core::http_data_istream_wrapper>(std::move(body)):nullptr);
        }
    private:
        static std::string string_to_sign_(const std::string& canonical_request)
        {
            return std::format("ACS3-HMAC-SHA256\n{}", boost::algorithm::hex_lower(sha_256_(canonical_request)));
        }
        static std::string canonical_request_(core::raw::http_request_raw& req, std::string& signed_headers)
        {
            std::string result;
            result.append(std::format("{}\n",req.header_.meta_.p1_));
            auto deli_pos = req.header_.meta_.p2_.find('?');
            std::string path = req.header_.meta_.p2_.substr(0, deli_pos);
            std::string query = deli_pos==std::string::npos? std::string{}:req.header_.meta_.p2_.substr(deli_pos+1);
            result.append(std::format("{}\n", path));
            result.append(std::format("{}\n", sahara::string_ext::url_encode(canonical_query_string_(query))));
            result.append(canonical_header_(req.header_, signed_headers));
            result.append(boost::algorithm::hex_lower(sha_256_(req.body_)));
            return result;
        }

        static std::string canonical_header_(core::raw::http_header_raw& header, std::string& signed_headers)
        {
            std::string result;
            std::string canonical_headers_;
            auto key_view = std::views::keys(header.headers_);
            std::vector<std::string> lowered_headers;
            for (auto &key: key_view)
                lowered_headers.emplace_back(boost::to_lower_copy(key));
            std::ranges::sort(lowered_headers);
            for (auto &lowered_header: lowered_headers){
                if (!signed_headers.empty())
                    signed_headers.append(";");
                signed_headers.append(std::format("{}",lowered_header));
                canonical_headers_.append(std::format("{}:{}\n", lowered_header, boost::algorithm::trim_copy(header.headers_[lowered_header])));
            }
            return std::format("{}{}\n", canonical_headers_, signed_headers);
        }
        static std::string get_signature_(std::chrono::system_clock::time_point timestamp, const std::string& data) {

            return "";
            //
            // std::string authorization_header = "Authorization: ACS3-HMAC-SHA256 "
            // std::string authorization = std::format(" Credential={},SignedHeaders=host;x-acs-action;x-acs-content-sha256;x-acs-date;x-acs-signature-nonce;x-acs-version,Signature=06563a9e1b43f5dfe96b81484da74bceab24a1d853912eee15083a6f0f3283c0", access_key_id_);
            // const std::string string_to_sign = std::format("OSS4-HMAC-SHA256\n{:%Y%m%dT%H%M%SZ}\n{:%Y%m%d}/{}/{}/aliyun_v4_request\n{}", timestamp,timestamp, region_, product_,  boost::algorithm::hex(hmac_sha_256_(canonical_, "aliyun_")));
            // const std::string DateKey = hmac_sha_256_("aliyun_v4" + access_key_secret_,std::format("{:%Y%m%dT%H%M%SZ}", timestamp));
            // const std::string DateRegionKey = hmac_sha_256_(DateKey, region_);
            // const std::string DateRegionServiceKey = hmac_sha_256_(DateRegionKey, product_);
            // const std::string SigningKey = hmac_sha_256_(DateRegionServiceKey, "aliyun_v4_request");
            // const std::string signature = hmac_sha_256_(SigningKey, data);
            // return boost::algorithm::hex(signature);
        }

        static std::string canonical_query_string_(const std::string& query)
        {
            if (query.empty()) return "";
            std::unordered_map<std::string ,nlohmann::json> params;
            parser_v3::parse_urlencoded_param(params, query);
            std::string result;
            for (auto & [first, second]: params)
                result.append(std::format("{}={}", first, second.dump()));
            return result;
        }

        static std::string hmac_sha_256_(const std::string&key, const std::string& data) {
            CryptoPP::HMAC<CryptoPP::SHA256> hmac(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size());
            std::string result;
            CryptoPP::StringSource ss(data, true, new CryptoPP::HashFilter(hmac,new CryptoPP::StringSink(result)));
            return result;
        }

        static std::string sha_256_(const std::string& data) {
            CryptoPP::SHA256 sha256;
            sha256.Update(reinterpret_cast<const CryptoPP::byte*>(data.data()), data.length());
            std::string result;
            CryptoPP::StringSource ss(data, true, new CryptoPP::HashFilter(sha256,new CryptoPP::StringSink(result)));
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

    private:
        std::string access_key_id_;
        std::string access_key_secret_;
        std::string region_;
        std::string endpoint_;
        std::string product_;
    };

}

#endif //ALIYUN_CLIENT_H
