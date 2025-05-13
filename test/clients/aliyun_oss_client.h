#ifndef ALIYUN_OSS_CLIENT_H
#define ALIYUN_OSS_CLIENT_H
#include <utility>

#include "obelisk/http/client/signer/aliyun_oss.h"
#include "obelisk/http/core/base_client.h"
#include "obelisk/http/framework/filesystem/base_fs.h"

class aliyun_oss_client final : public obelisk::http::base_fs{
public:

    aliyun_oss_client(std::string ak_id, std::string ak_secret, std::string region, std::string bucket):
        http_client_(std::make_unique<obelisk::http::client::signer::aliyun_oss>(ak_id, ak_secret, region, bucket)),ak_id_(std::move(ak_id)), ak_secret_(std::move(ak_secret)), bucket_(std::move(bucket)), region_(std::move(region))
    {
    }
    boost::asio::awaitable<bool> exists(const std::string& path) override
    {
        const std::string url = std::format("https://{}.oss-{}.aliyuncs.com/{}", bucket_, region_, path);
        const auto resp = co_await http_client_.send_request(url, "HEAD", {}, nullptr);
        if (resp->header_raw().meta_.p2_ == "200")
            co_return true;
        if (resp->header_raw().meta_.p2_ == "404")
            co_return false;
        throw std::runtime_error(resp->header_raw().meta_.p3_);
    }
    boost::asio::awaitable<bool> save(const std::string& path, std::unique_ptr<std::iostream> file) override
    {
        std::string url = std::format("https://{}.oss-{}.aliyuncs.com/{}", bucket_, region_, path);
        auto resp = co_await http_client_.send_request(url, "PUT", {}, file? std::make_unique<obelisk::http::core::http_data_istream_wrapper>(std::move(file)): nullptr);
        co_return true;
    }
    boost::asio::awaitable<bool> remove(const std::string& path) override
    {
        const std::string url = std::format("http://{}.oss-{}.aliyuncs.com/{}", bucket_, region_, path);
        const auto resp = co_await http_client_.send_request(url, "DELETE", {}, nullptr);
        if (resp->header_raw().meta_.p2_.starts_with("20"))
            co_return true;
        throw std::runtime_error(resp->header_raw().meta_.p3_);
    }
private:
    void sign_(obelisk::http::core::raw::http_request_raw& raw)
    {

    }
    obelisk::http::core::base_client http_client_;

    std::string ak_id_;
    std::string ak_secret_;
    std::string bucket_;
    std::string region_;
};



#endif //ALIYUN_OSS_CLIENT_H
