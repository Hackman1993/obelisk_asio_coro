//
// Created by Hackman.Lo on 2024/2/26.
//

#include "aliyun_oss.h"

#include <sahara/string/string_ext.h>
#include <sahara/utils/uuid.h>

#include "operation/aliyun_oss/put_object.h"
#include "operation/aliyun_oss/delete_object.h"
#include "operation/aliyun_oss/delete_object_tags.h"
#include "operation/aliyun_oss/object_exists.h"
#include "operation/aliyun_oss/set_object_tags.h"

namespace obelisk::storage::impl::alioss {
    std::atomic_uint aliyun_oss_instance::ref_count_ = 0;

    aliyun_oss_instance::aliyun_oss_instance() {
        if (!ref_count_ == 0)
            InitializeSdk();
        ++ref_count_;

        ClientConfiguration conf;
        client_ = std::make_shared<OssClient>("oss-cn-chengdu.aliyuncs.com", "","", conf);
        bucket_ = "hl-blog-bucket";
    }

    std::shared_ptr<delete_operation> aliyun_oss_instance::remove(const std::string& path) {
        return std::make_shared<delete_object_operation>(*client_, bucket_, path);
    }

    std::shared_ptr<save_operation> aliyun_oss_instance::save(const std::string&path, const std::shared_ptr<std::iostream>& stream, const std::unordered_map<std::string, std::string>& tags) {
        return std::make_shared<put_object_operation>(*client_, bucket_, path, stream, tags);
    }

    std::shared_ptr<exists_operation> aliyun_oss_instance::exists(const std::string& path) {
        return std::make_shared<object_exists_operation>(*client_, bucket_, path);
    }

    std::shared_ptr<set_tag_operation> aliyun_oss_instance::set_tag(const std::string& path, const std::unordered_map<std::string, std::string>& tag) {
        return std::make_shared<set_object_tags>(*client_, bucket_, path, tag);
    }

    std::shared_ptr<delete_tag_operation> aliyun_oss_instance::delete_tag(const std::string& path) {
        return std::make_shared<delete_object_tags>(*client_, bucket_, path);
    }
} // obelisk::storage
