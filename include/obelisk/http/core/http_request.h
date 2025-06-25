#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H
#include <any>
#include <boost/asio/awaitable.hpp>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "raw.h"
#include "../validator/validator_base.h"

namespace obelisk::http {
    class parser_v2;
    class parser_v3;
    class http_connection;

    class http_temp_fstream final : public std::fstream {
    public:
        explicit http_temp_fstream(std::string path);
        ~http_temp_fstream() override;
        std::string path();
    protected:
        std::string path_;
    };

    struct http_file {
        http_file(std::string tmp_path, std::string filename): temp_path_(std::move(tmp_path)), filename_(std::move(filename)) {
            if(const std::filesystem::path file(filename_); file.has_extension()) extension_ = file.extension().string();
        }
        std::string temp_path_;
        std::string filename_;
        std::string mime_type_;
        std::optional<std::string> extension_;
        ~http_file() {
            if (std::filesystem::exists(temp_path_))
                std::filesystem::remove(temp_path_);
        }
    };

    class http_request {
    public:

        void set_param(const std::string&name, const std::string&value);

        std::shared_ptr<http_file>& file(const std::string&file);

        const std::vector<std::string>& param(const std::string&name);

        [[nodiscard]] const std::string& method() const;

        [[nodiscard]] const std::string& target() const;

        [[nodiscard]] const std::string_view& path() const;

        [[nodiscard]] const std::string& protocol() const;

        [[nodiscard]] std::uint64_t content_length() const;

        [[nodiscard]] bool has_header(const std::string&name) const;

        [[nodiscard]] bool contains_file(const std::string&file) const;

        [[nodiscard]] bool contains_param(const std::string&name) const;

        [[nodiscard]] const std::string& header(const std::string&name) const;

        const std::string_view& content_type();

        std::unordered_map<std::string, std::string> route_params_;
        std::unordered_map<std::string, std::shared_ptr<http_file>> filebag_;

    protected:
        core::raw::http_header_raw header_;
        std::string_view path_;
        std::string_view content_type_;
        std::uint64_t content_length_ = 0;
        std::unordered_map<std::string, std::vector<std::string>> params_;
    };

    class http_request_wrapper {
    public:
        http_request_wrapper(boost::asio::io_context& ioctx, http_header&header, std::unique_ptr<std::iostream> raw_body = nullptr);
        http_request_wrapper(boost::asio::io_context& ioctx, core::raw::http_header_raw& header, std::unique_ptr<std::iostream> raw_body = nullptr);

        ~http_request_wrapper();

        boost::asio::awaitable<void> validate(const std::vector<validator::validator_group>& validators);

        sahara::container::unordered_smap_u<std::string>& headers();

        [[nodiscard]] std::string_view version() const;

        void set_version(const std::string&version);

        [[nodiscard]] std::string_view path() const;

        [[nodiscard]] std::string_view target() const;

        [[nodiscard]] std::string_view method() const;

        [[nodiscard]] std::string_view query_string() const;

        std::shared_ptr<std::iostream>& raw_body();

        std::unordered_map<std::string, std::any>& registered_data();

        nlohmann::json::object_t& params();

        std::unordered_map<std::string, std::shared_ptr<http_file>>& filebag();

        boost::asio::io_context& io_context() {
            return ioctx_;
        }

        std::unordered_map<std::string, std::any>& additional_data() { return additional_data_; };

    protected:
        core::raw::http_header_raw header_raw_;

        std::string_view target_;
        boost::asio::io_context& ioctx_;
        std::shared_ptr<std::iostream> raw_body_;
        std::unordered_map<std::string, std::any> registered_value_;
        std::unordered_map<std::string, std::shared_ptr<http_file>> filebag_;
        nlohmann::json::object_t request_params_;
        std::unordered_map<std::string, std::any> additional_data_;

        friend class parser_v2;
        friend class parser_v3;
    };
} // core

#endif //OBELISK_HTTP_REQUEST_H
