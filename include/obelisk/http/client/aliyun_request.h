// #ifndef OBELISK_HTTP_CLIENT_ALIYUN_REQUEST_H
// #define OBELISK_HTTP_CLIENT_ALIYUN_REQUEST_H
// #include <iostream>
//
// #include "request.h"
// #include <cryptopp/sha.h>
// #include <cryptopp/hmac.h>
// #include <cryptopp/hex.h>
// #include <boost/algorithm/hex.hpp>
// namespace obelisk::http::client
// {
//     class aliyun_request: public request {
//     public:
//         aliyun_request(const std::string& uri, const std::string& method): request(uri, method)
//         {
//             canonical_ = method + "\n";
//             std::vector<std::string> querys;
//             boost::split(querys, query_, [](const char c) { return c == '&'; });
//
//         }
//
//         boost::asio::awaitable<std::string> send(){
//             using namespace boost::asio;
//             auto executor = co_await this_coro::executor;
//             ip::tcp::resolver resolver(executor);
//             auto [ec, endpoint] = co_await resolver.async_resolve(host_, boost::lexical_cast<std::string>(port_), as_tuple(use_awaitable));
//
//             if (protocol_ == "https")
//             {
//                 ssl::context ssl_context(ssl::context::tlsv12_client);
//                 ssl_context.set_default_verify_paths();
//                 ssl_context.set_verify_mode(ssl::context::verify_peer);
//                 ssl::stream<ip::tcp::socket> ssl_socket(executor, ssl_context);
//                 if (auto [connect_ec, connect_ep] = co_await async_connect(ssl_socket.next_layer(), endpoint, as_tuple(use_awaitable)); connect_ec)
//                     throw connect_ec;
//                 co_await ssl_socket.async_handshake(ssl::stream_base::client, use_awaitable);
//                 co_await write_resp(ssl_socket);
//             }else
//             {
//                 ip::tcp::socket socket(executor);
//                 if (auto [connect_ec, connect_ep] = co_await async_connect(socket, endpoint, as_tuple(use_awaitable)); connect_ec)
//                     throw connect_ec;
//                 co_await write_resp(socket);
//             }
//
//
//             co_return "";
//         }
//
//     protected:
//
//         template <typename StreamType>
//         boost::asio::awaitable<std::string> write_resp(StreamType& t)
//         {
//             using namespace boost::asio;
//             // co_await async_write(t, )
//             co_return "";
//         }
//         void before_send() override {
//             auto timestamp = std::chrono::system_clock::now();
//             canonical_ += header_.meta_.p2_ + "\n";
//             canonical_ += "\n";
//             canonical_ += "\n";
//             canonical_ += "\n";
//             canonical_ += "UNSIGNED-PAYLOAD";
//
//             const std::string DateKey = hmac_sha_256("aliyun_v4g95zy5DiRxXcpL7L3BAbClvjTr5HoX","20250411");
//             const std::string DateRegionKey = hmac_sha_256(DateKey, "cn-hangzhou");
//             const std::string DateRegionServiceKey = hmac_sha_256(DateRegionKey, "oss");
//             const std::string SigningKey = hmac_sha_256(DateRegionServiceKey, "aliyun_v4_request");
//             std::cout << boost::algorithm::hex(SigningKey) << std::endl;
//         }
//
//
//             header_.headers_.emplace("Date", std::format("{:%a, %d %b %Y %H:%M:%S GMT}", timestamp));
//             auto signature = get_signature_(access_key_secret_, region_, product_, timestamp,"");
//         }
//
//         std::string get_signature_(const std::string& secret,const std::string& region, const std::string& product, std::chrono::system_clock::time_point timestamp, const std::string& data) {
//
//             const std::string string_to_sign = std::format("OSS4-HMAC-SHA256\n{:%Y%m%dT%H%M%SZ}\n{:%Y%m%d}/{}/{}/aliyun_v4_request\n{}", timestamp,timestamp, region, product_,  boost::algorithm::hex(hmac_sha_256_(canonical_, "aliyun_")));
//
//             const std::string DateKey = hmac_sha_256_("aliyun_v4" + secret,std::format("{:%Y%m%dT%H%M%SZ}", timestamp));
//             const std::string DateRegionKey = hmac_sha_256_(DateKey, region);
//             const std::string DateRegionServiceKey = hmac_sha_256_(DateRegionKey, product);
//             const std::string SigningKey = hmac_sha_256_(DateRegionServiceKey, "aliyun_v4_request");
//             const std::string signature = hmac_sha_256_(SigningKey, data);
//             return boost::algorithm::hex(signature);
//         }
//         std::string hmac_sha_256_(const std::string&key, const std::string& data) {
//             CryptoPP::HMAC<CryptoPP::SHA256> hmac(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size());
//             std::string result;
//             CryptoPP::StringSource ss(data, true, new CryptoPP::HashFilter(hmac,new CryptoPP::StringSink(result)));
//             return result;
//         }
//         std::string sha_256_(const std::string& data) {
//             CryptoPP::SHA256 sha256;
//             sha256.Update(reinterpret_cast<const CryptoPP::byte*>(data.data()), data.length());
//             std::string result;
//             CryptoPP::StringSource ss(data, true, new CryptoPP::HashFilter(sha256,new CryptoPP::StringSink(result)));
//             return result;
//         }
//         std::string canonical_;
//         std::string region_;
//         std::string endpoint_;
//         std::string access_key_id_;
//         std::string access_key_secret_;
//         std::string product_;
//         std::string scope_;
//     };
//
// }
//
// #endif //OBELISK_HTTP_CLIENT_ALIYUN_REQUEST_H
