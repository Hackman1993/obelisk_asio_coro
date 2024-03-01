//
// Created by Hackman.Lo on 2024/2/27.
//

#ifndef PUT_OBJECT_H
#define PUT_OBJECT_H

#include <utility>

#include "../../aliyun_oss.h"
using namespace AlibabaCloud::OSS;
namespace obelisk::storage::impl::alioss {

    struct put_object_operation : save_operation {
        put_object_operation(const OssClient&client, std::string bucket, std::string key, std::shared_ptr<std::iostream>stream, const std::unordered_map<std::string, std::string>& tags);

        void await_suspend(std::coroutine_handle<> coro) override ;
    private:
        OssClient client_;
        std::string bucket_;
    };

} // obelisk::storage::impl::alioss

#endif //PUT_OBJECT_H
