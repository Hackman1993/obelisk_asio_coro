//
// Created by Hackman.Lo on 2024/2/26.
//

#ifndef STORAGE_INSTANCE_H
#define STORAGE_INSTANCE_H
#include <core/coroutine/awaitable_operation.h>

#include <utility>
#include <alibabacloud/oss/OssClient.h>
#include <sahara/exception/exception_base.h>

namespace obelisk::storage::impl {
//    class storage_error final : public sahara::exception::exception_base {
//    public:
//        explicit storage_error(const std::string&message): exception_base(message) {}
//    };
//
//    template<typename ResultType>
//    struct operation_result {
//        bool success_ = false;
//        ResultType result_{};
//        std::shared_ptr<storage_error> error_;
//    };
//
//    template<typename Result>
//    struct storage_operation : core::coroutine::awaitable_operation<operation_result<Result>> {
//        explicit storage_operation(std::string path): path_(std::move(path)){}
//        [[nodiscard]] operation_result<Result> await_resume() const noexcept override {
//            return result_;
//        }
//    protected:
//        std::string path_;
//        operation_result<Result> result_;
//    };
//
//    struct save_operation : storage_operation<std::string> {
//        save_operation(std::string path, std::shared_ptr<std::iostream> stream, const std::unordered_map<std::string, std::string>& tags)
//            : storage_operation(std::move(path)),stream_(std::move(stream)), tags_(tags) {
//        }
//        std::shared_ptr<std::iostream> stream_;
//        const std::unordered_map<std::string, std::string>& tags_;
//    };
//
//    struct delete_operation : storage_operation<std::string> {
//        explicit delete_operation(std::string path) : storage_operation(std::move(path)){}
//    };
//
//    struct set_tag_operation: storage_operation<std::string> {
//        explicit set_tag_operation(std::string path, const std::unordered_map<std::string, std::string>& tags) : storage_operation(std::move(path)), tags_(tags){}
//    protected:
//        const std::unordered_map<std::string, std::string>& tags_;
//    };
//    struct delete_tag_operation: storage_operation<std::string> {
//        explicit delete_tag_operation(std::string path) : storage_operation(std::move(path)){}
//    };
//
//    struct exists_operation : storage_operation<bool> {
//        explicit exists_operation(std::string path) : storage_operation(std::move(path)){}
//    };
//
//    class storage_instance {
//    public:
//        virtual std::shared_ptr<delete_operation> remove(const std::string& path) = 0;
//
//        virtual std::shared_ptr<exists_operation> exists(const std::string& path) = 0;
//        virtual std::shared_ptr<set_tag_operation> set_tag(const std::string& path, const std::unordered_map<std::string, std::string>& tag) = 0;
//        virtual std::shared_ptr<delete_tag_operation> delete_tag(const std::string& path) = 0;
//
//        virtual std::shared_ptr<save_operation> save(const std::string&path, const std::shared_ptr<std::iostream>& stream, const std::unordered_map<std::string, std::string>& tags) = 0;
//        virtual ~storage_instance() = default;
//    };
} // obelisk::storage

#endif //STORAGE_INSTANCE_H
