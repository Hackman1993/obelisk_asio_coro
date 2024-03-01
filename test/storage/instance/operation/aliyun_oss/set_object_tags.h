//
// Created by Hackman.Lo on 2024/2/27.
//

#ifndef SET_OBJECT_TAGS_H
#define SET_OBJECT_TAGS_H
#include "../../aliyun_oss.h"


namespace obelisk::storage::impl::alioss {
    struct set_object_tags final : oss_operation, set_tag_operation {
        set_object_tags(OssClient&client, std::string bucket, std::string key, const std::unordered_map<std::string, std::string>&tags);

        void await_suspend(std::coroutine_handle<> coro) override;
    };
} // obelisk::storage::impl

#endif //SET_OBJECT_TAGS_H
