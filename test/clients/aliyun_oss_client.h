//
// Created by hackman on 5/9/25.
//

#ifndef ALIYUN_OSS_CLIENT_H
#define ALIYUN_OSS_CLIENT_H

#include <obelisk/http/client/aliyun_client.h>

#include "obelisk/http/framework/filesystem/base_fs.h"

class aliyun_oss_client : public obelisk::http::base_fs{
public:

    aliyun_oss_client(std::string ak_id, std::string ak_secret, std::string region, std::string endpoint, std::string bucket):
        http_client_(std::move(ak_id), std::move(ak_secret), std::move(region), std::move(endpoint)), bucket_(std::move(bucket))
    {
    }
    boost::asio::awaitable<bool> exists(const std::string& path) override
    {
        std::string url = std::format("https://{}.oss-cn-chengdu.aliyuncs.com/{}", bucket_, path);
        auto resp = co_await http_client_.send_request(url, "HEAD", {}, nullptr, "2019-05-17", "HeadObject");
        if (resp->header_raw().meta_.p2_ == "200")
            co_return true;
        if (resp->header_raw().meta_.p2_ == "404")
            co_return false;
        throw std::runtime_error(resp->header_raw().meta_.p3_);
    }
    boost::asio::awaitable<bool> save(const std::string& path, const std::istream& file) override{co_return true;}
    boost::asio::awaitable<bool> remove(const std::string& path) override{co_return true;}
private:
    std::string bucket_;
    obelisk::http::client::aliyun_client http_client_;
};



#endif //ALIYUN_OSS_CLIENT_H
