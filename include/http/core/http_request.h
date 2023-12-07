//
// Created by Hackman.Lo on 10/12/2023.
//

#ifndef OBELISK_HTTP_REQUEST_H
#define OBELISK_HTTP_REQUEST_H
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <fstream>
#include <variant>
#include <filesystem>
#include <unordered_map>

namespace obelisk::http {
    class http_connection;
    class http_block_data;
    class http_temp_fstream : public std::fstream{
    public:
        explicit http_temp_fstream(std::string path);
        ~http_temp_fstream() override;
        std::string path();
    protected:
        std::string path_;
    };

    struct http_file{
        http_file(std::string tmp_path, std::string filename): temp_path_(std::move(tmp_path)), filename_(std::move(filename)){}
        std::string temp_path_;
        std::string filename_;
        ~http_file(){
            if(std::filesystem::exists(temp_path_))
                std::filesystem::remove(temp_path_);
        }
    };

    class http_request{
    public:
        http_request(std::shared_ptr<http_block_data>& data);
        void set_param(const std::string& name, const std::string& value);

        std::shared_ptr<http_file>& file(const std::string& file);
        const std::vector<std::string>& param(const std::string& name);
        [[nodiscard]] const std::string& method() const;
        [[nodiscard]] const std::string& target() const;
        [[nodiscard]] const std::string_view& path() const;
        [[nodiscard]] const std::string& protocol() const;
        [[nodiscard]] std::uint64_t content_length() const;
        [[nodiscard]] bool has_header(const std::string& name) const;
        [[nodiscard]] bool contains_file(const std::string& file) const;
        [[nodiscard]] bool contains_param(const std::string& name) const;
        [[nodiscard]] const std::string& header(const std::string& name) const;
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

} // core

#endif //OBELISK_HTTP_REQUEST_H