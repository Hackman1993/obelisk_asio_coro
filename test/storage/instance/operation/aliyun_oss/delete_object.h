//
// Created by Hackman.Lo on 2024/2/27.
//

#ifndef DELETE_OBJECT_H
#define DELETE_OBJECT_H
#include "../../aliyun_oss.h"

namespace obelisk::storage::impl::alioss {
    using namespace AlibabaCloud::OSS;

    struct delete_object_operation : delete_operation, oss_operation {
        delete_object_operation(OssClient&client, std::string bucket, std::string key);

        void await_suspend(std::coroutine_handle<> coro) override;
    };
} // obelisk::storage::impl::alioss

#endif //DELETE_OBJECT_H
