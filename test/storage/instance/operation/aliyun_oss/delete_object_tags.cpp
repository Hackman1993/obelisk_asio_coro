//
// Created by Hackman.Lo on 2024/2/27.
//

#include "delete_object_tags.h"

namespace obelisk::storage::impl::alioss {
    delete_object_tags::delete_object_tags(OssClient&client, std::string bucket, std::string key)
        : oss_operation(client, std::move(bucket)), delete_tag_operation(std::move(key)) {
    }

    void delete_object_tags::await_suspend(std::coroutine_handle<> coro) {
        std::thread([&, coro] {
            DeleteObjectTaggingRequest request(bucket_, path_);
            auto result = client_.DeleteObjectTagging(request);
            if (!result.isSuccess())
                result_.error_ = std::make_shared<storage_error>(result.error().Message());
            result_.success_ = result.isSuccess();
            coro.resume();
        }).detach();
    }
} // obelisk::storage::impl::alioss
