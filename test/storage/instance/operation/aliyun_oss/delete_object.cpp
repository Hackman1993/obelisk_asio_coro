//
// Created by Hackman.Lo on 2024/2/27.
//

#include "delete_object.h"

#include <utility>


namespace obelisk::storage::impl::alioss {
    delete_object_operation::delete_object_operation(OssClient&client, std::string bucket, std::string key)
        : delete_operation(std::move(key)), oss_operation(client, std::move(bucket)) {

    }

    void delete_object_operation::await_suspend(std::coroutine_handle<> coro)  {
        std::thread([&, coro] {
            if (const auto result = client_.DeleteObject(bucket_, path_); !result.isSuccess())
                result_.error_ = std::make_shared<storage_error>(result.error().Message());
            coro.resume();
        }).detach();
    }
} // obelisk::obelisk::storage::impl::alioss