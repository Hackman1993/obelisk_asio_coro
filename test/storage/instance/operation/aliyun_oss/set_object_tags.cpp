//
// Created by Hackman.Lo on 2024/2/27.
//

#include "set_object_tags.h"

#include <utility>


namespace obelisk::storage::impl::alioss {
    set_object_tags::set_object_tags(OssClient&client, std::string bucket, std::string key, const std::unordered_map<std::string, std::string>&tags)
        : oss_operation(client, std::move(bucket)), set_tag_operation(std::move(key), tags) {
    }

    void set_object_tags::await_suspend(std::coroutine_handle<> coro) {
        std::thread([&, coro] {
            Tagging tagging;
            for (const auto&tag: tags_) {
                tagging.addTag(Tag{tag.first, tag.second});
            }
            SetObjectTaggingRequest request(bucket_, path_, tagging);
            auto result = client_.SetObjectTagging(request);
            if(!result.isSuccess())
                result_.error_ = std::make_shared<storage_error>(result.error().Message());
            result_.success_ = result.isSuccess();
            coro.resume();
        }).detach();

    }
} // obelisk::storage::impl::alioss
