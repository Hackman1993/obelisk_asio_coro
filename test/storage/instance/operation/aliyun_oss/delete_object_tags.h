//
// Created by Hackman.Lo on 2024/2/27.
//

#ifndef DELETE_OBJECT_TAGS_H
#define DELETE_OBJECT_TAGS_H
#include "../../aliyun_oss.h"


namespace obelisk::storage::impl::alioss {
    struct delete_object_tags final : oss_operation, delete_tag_operation {
        delete_object_tags(OssClient&client, std::string bucket, std::string key);

        void await_suspend(std::coroutine_handle<> coro) override;
    };
} // obelisk::storage::impl

#endif //DELETE_OBJECT_TAGS_H
