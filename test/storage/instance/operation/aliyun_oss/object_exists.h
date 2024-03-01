//
// Created by Hackman.Lo on 2024/2/27.
//

#ifndef OBJECT_EXISTS_H
#define OBJECT_EXISTS_H

#include "../../aliyun_oss.h"
namespace obelisk::storage::impl::alioss {
    using namespace AlibabaCloud::OSS;
    struct object_exists_operation final: exists_operation, oss_operation {
        object_exists_operation(OssClient&client, std::string bucket, std::string path);

        void await_suspend(std::coroutine_handle<> coro) override;
    };
} // obelisk::storage::impl::alioss

#endif //OBJECT_EXISTS_H
