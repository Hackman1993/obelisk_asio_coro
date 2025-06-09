#ifndef ALIYUN_OSS_CLIENT_H
#define ALIYUN_OSS_CLIENT_H
#include <utility>

#include "obelisk/http/client/signer/aliyun_oss.h"
#include "obelisk/http/core/base_client.h"
#include "obelisk/filesystem/detail/base_filesystem.h"

class aliyun_oss_client final : public obelisk::fs::detail::base_filesystem{
public:
    boost::asio::awaitable<std::string>
    save_random_name(const std::string& path, obelisk::http::http_file& file) override
    {
        co_return "";
    }

    aliyun_oss_client(const std::string& ak_id, const std::string& ak_secret, std::string region, std::string bucket):
        http_client_(std::make_unique<obelisk::http::client::signer::aliyun_oss>(ak_id, ak_secret, region, bucket)),
        bucket_(std::move(bucket)), region_(std::move(region))
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
    boost::asio::awaitable<std::string> save(const std::string& path, std::unique_ptr<std::iostream> file) override
    {
        std::string url = std::format("https://{}.oss-{}.aliyuncs.com/{}", bucket_, region_, path);
        auto resp = co_await http_client_.send_request(url, "PUT", {}, file? std::make_unique<obelisk::http::core::http_data_istream_wrapper>(std::move(file)): nullptr);
        co_return url;
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
    obelisk::http::core::base_client http_client_;
    std::string bucket_;
    std::string region_;
};



#endif //ALIYUN_OSS_CLIENT_H
