//
// Created by Hackman.Lo on 2024/2/26.
//

#ifndef ALIYUN_OSS_H
#define ALIYUN_OSS_H
#include <utility>
#include <core/coroutine/awaitable_operation.h>
#include <alibabacloud/oss/OssClient.h>
#include "../storage_instance.h"

namespace obelisk::storage::impl::alioss {
    using namespace AlibabaCloud::OSS;

    struct oss_operation {
        oss_operation(OssClient& client, std::string bucket): client_(client), bucket_(std::move(bucket)){}
    protected:
        OssClient& client_;
        std::string bucket_;
    };

    class aliyun_oss_instance final : public storage_instance {
    public:
        aliyun_oss_instance();

        ~aliyun_oss_instance() override {
            --ref_count_;
            if (ref_count_ == 0)
                ShutdownSdk();
        }

        std::shared_ptr<impl::delete_operation> remove(const std::string& path) override;

        std::shared_ptr<impl::save_operation> save(const std::string&path, const std::shared_ptr<std::iostream>& stream, const std::unordered_map<std::string, std::string>& tags) override;

        std::shared_ptr<impl::exists_operation> exists(const std::string& path) override;

        std::shared_ptr<set_tag_operation> set_tag(const std::string& path, const std::unordered_map<std::string, std::string>& tag) override;

        std::shared_ptr<delete_tag_operation> delete_tag(const std::string& path) override;

    private:
        static std::atomic_uint ref_count_;
        std::shared_ptr<OssClient> client_;
        std::string bucket_;
    };

    // struct oss_configuration {
    //     std::string endpoint;
    //     std::string access_key_id;
    //     std::string access_secret;
    //     ClientConfiguration configuration;
    // };
    //
    // struct aliyun_oss_operation_base {
    //     aliyun_oss_operation_base();
    // private:
    //     static std::atomic_bool initialized_;
    // };
    //
    // struct put_object_awaitable: aliyun_oss_operation_base, core::coroutine::awaitable_operation<PutObjectOutcome> {
    //     void await_suspend(std::coroutine_handle<> coro) override;
    //
    //     [[nodiscard]] Outcome<OssError, PutObjectResult> await_resume() const noexcept override;
    // };
    // struct delete_object_awaitable: aliyun_oss_operation_base, core::coroutine::awaitable_operation<PutObjectOutcome> {
    //     void await_suspend(std::coroutine_handle<> coro) override;
    //
    //     [[nodiscard]] Outcome<OssError, PutObjectResult> await_resume() const noexcept override;
    // };
    //
    // class oss_instance {
    // public:
    //     virtual ~oss_instance();
    //     virtual put_object_awaitable put_object();
    //     virtual delete_object_awaitable delete_object();
    //
    //
    // private:
    //     oss_configuration configuration_;
    // };
    //
    // class aliyun_oss {
    // public:
    // private:
    //     static std::unordered_map<std::string, oss_instance> configuration;
    // };
} // obelisk

#endif //ALIYUN_OSS_H
