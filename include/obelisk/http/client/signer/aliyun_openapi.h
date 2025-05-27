#ifndef ALIYUN_OPENAPI_H
#define ALIYUN_OPENAPI_H
#include <ranges>
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <cryptopp/filters.h>
#include <nlohmann/json.hpp>
#include <obelisk/http/parser/http_parser_v3.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include "base_signer.h"

namespace obelisk::http::client::signer
{
    using namespace boost::algorithm;
    class aliyun_openapi final : public base_signer {
    public:
        aliyun_openapi(std::string ak_id, std::string ak_secret): access_key_id_(std::move(ak_id)), ak_secret_(std::move(ak_secret)){}
        void sign_request(core::raw::http_request_raw& raw) override
        {
            auto &header = raw.header_;
            if (raw.body_)
                header.headers_.emplace("x-acs-content-sha256", sha_256_(raw.body_));
            else header.headers_.emplace("x-acs-content-sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
            std::chrono::system_clock::time_point send_time = std::chrono::system_clock::now();
            header.headers_.emplace("x-acs-date", std::format("{:%Y-%m-%dT%H:%M:%OSZ}", send_time));
            header.headers_.emplace("x-acs-signature-nonce", replace_all_copy(sahara::utils::uuid::generate(),"-", ""));
            std::string canonical_request;
            canonical_request.append(std::format("{}\n", header.meta_.p1_));
            const auto deli_pos = header.meta_.p2_.find('?');
            std::string path = header.meta_.p2_.substr(0, deli_pos);
            const std::string query = deli_pos==std::string::npos? std::string{}:header.meta_.p2_.substr(deli_pos+1);
            canonical_request.append(std::format("{}\n", path));
            canonical_request.append(std::format("{}\n", canonical_query_string_(query)));
            std::vector<std::string> signed_headers;
            for (const auto& first : header.headers_ | std::views::keys){
                if (auto lowered_header = to_lower_copy(first); lowered_header.starts_with("x-acs-") || lowered_header=="host" || lowered_header == "content-type")
                    signed_headers.emplace_back(std::move(lowered_header));
            }
            std::ranges::sort(signed_headers);
            std::string canonical_header_str;
            std::string signed_headers_str;
            for (int i = 0; i< signed_headers.size();++i){
                if (i > 0) signed_headers_str.append(";");
                signed_headers_str.append(signed_headers[i]);
                canonical_header_str.append(std::format("{}:{}\n", signed_headers[i], raw.header_.headers_[signed_headers[i]]));
            }
            canonical_request.append(std::format("{}\n", canonical_header_str));
            canonical_request.append(std::format("{}\n", signed_headers_str));
            canonical_request.append(raw.header_.headers_["x-acs-content-sha256"]);

            const std::string string_to_sign = std::format("ACS3-HMAC-SHA256\n{}", hex_lower(sha_256_(canonical_request)));
            std::string signature = hex_lower(hmac_sha_256_(ak_secret_,string_to_sign));

            std::string authorization_header = std::format("ACS3-HMAC-SHA256 Credential={},SignedHeaders={},Signature={}",
                access_key_id_, signed_headers_str ,signature);
            raw.header_.headers_.emplace("Authorization", authorization_header);
        }

    private:
        static std::string canonical_query_string_(const std::string& query)
        {
            if (query.empty()) return "";
            nlohmann::json::object_t params;
            parser_v3::parse_urlencoded_param(params, query);

            std::vector<std::string> sorted;
            for (const auto& item : params)
                sorted.emplace_back(item.first);
            std::ranges::sort(sorted);

            std::string result;
            for (auto &key: sorted){
                if (!result.empty()) result.append("&");
                auto encoded =  sahara::string_ext::url_encode(params[key].get<std::string>());
                replace_all(encoded, "{", "%7B");
                replace_all(encoded, "}", "%7D");
                replace_all(encoded, ":", "%3A");
                replace_all(encoded, "\"", "%22");
                result.append(std::format("{}={}", key, encoded));
            }
            return result;
        }

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

        std::string access_key_id_;
        std::string ak_secret_;
    };

}

#endif //ALIYUN_OPENAPI_H
