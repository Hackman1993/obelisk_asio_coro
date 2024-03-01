//
// Created by Hackman.Lo on 2024/2/27.
//

#include "put_object.h"


namespace obelisk::storage::impl::alioss {
    put_object_operation::put_object_operation(const OssClient& client, std::string bucket, std::string key, std::shared_ptr<std::iostream> stream, const std::unordered_map<std::string, std::string>& tags)
        : save_operation(std::move(key), std::move(stream), tags), client_(client), bucket_(std::move(bucket)) {
    }

    void put_object_operation::await_suspend(std::coroutine_handle<> coro) {
        std::thread([&, coro] {
            PutObjectRequest request(bucket_, path_, stream_);

            std::shared_ptr<storage_error> error;
            Tagging tagging;
            for(const auto& [key, val]: tags_) {
                tagging.addTag(Tag{key, val});
            }
            request.setTagging(tagging.toQueryParameters());
            const auto result = client_.PutObject(request);
            if (!result.isSuccess())
                result_.error_ = std::make_shared<storage_error>(result.error().Message());
            result_.success_ = result.isSuccess();
            coro.resume();
        }).detach();
    }
} // obelisk::storage::impl::alioss