//
// Created by Hackman.Lo on 2024/2/27.
//

#include "object_exists.h"

#include <utility>

namespace obelisk {
namespace storage::impl::alioss {
    object_exists_operation::object_exists_operation(OssClient& client, std::string bucket, std::string key)
        : exists_operation(std::move(key)), oss_operation(client, bucket) {
    }

    void object_exists_operation::await_suspend(std::coroutine_handle<> coro) {
    }
} //
} // obelisk::storage::impl::alioss