#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H
#include <any>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "http_block_data.h"
#include "http/validator/validator_base.h"

namespace obelisk::http {
    class parser;
    class http_connection;
    class http_block_data;

    class http_temp_fstream : public std::fstream {
    public:
        explicit http_temp_fstream(std::string path);

        ~http_temp_fstream() override;

        std::string path();

    protected:
        std::string path_;
    };

    struct http_file {
        http_file(std::string tmp_path, std::string filename): temp_path_(std::move(tmp_path)),
                                                               filename_(std::move(filename)) {
        }

        std::string temp_path_;
        std::string filename_;

        ~http_file() {
            if (std::filesystem::exists(temp_path_))
                std::filesystem::remove(temp_path_);
        }
    };

    class http_request {
    public:
        http_request(std::shared_ptr<http_block_data>&data);

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

        std::shared_ptr<http_block_data>& raw();

        const std::string_view& content_type();

        std::unordered_map<std::string, std::string> route_params_;
        std::unordered_map<std::string, std::shared_ptr<http_file>> filebag_;

    protected:
        std::shared_ptr<http_block_data> data_;
        std::string_view path_;
        std::string_view content_type_;
        std::uint64_t content_length_ = 0;
        std::unordered_map<std::string, std::vector<std::string>> params_;
    };

    class http_request_wrapper {
    public:
        explicit http_request_wrapper(http_header&header, std::unique_ptr<std::iostream> raw_body = nullptr);

        ~http_request_wrapper();

        boost::asio::awaitable<void> validate(const std::vector<validator::validator_group>&validators);

        std::string_view header(const std::string&header);

        void set_header(const std::string&header, const std::string&value);

        [[nodiscard]] std::string_view version() const;

        void set_version(const std::string&version);

        [[nodiscard]] std::string_view path() const;

        [[nodiscard]] std::string_view target() const;

        [[nodiscard]] std::string_view method() const;

        [[nodiscard]] std::string_view query_string() const;

        std::unique_ptr<std::iostream>& raw_body();

        std::unordered_map<std::string, std::any>& registered_data();

        std::unordered_map<std::string, std::vector<std::string>>& params();

        std::unordered_map<std::string, std::shared_ptr<http_file>>& filebag();

    protected:
        std::string_view target_;
        http_header raw_header_;
        std::unique_ptr<std::iostream> raw_body_;
        std::unordered_map<std::string, std::any> registered_value_;
        std::unordered_map<std::string, std::shared_ptr<http_file>> filebag_;
        std::unordered_map<std::string, std::vector<std::string>> request_params_;

        friend class obelisk::http::parser;
    };
} // core

#endif //OBELISK_HTTP_REQUEST_H
