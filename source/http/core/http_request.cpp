#include "http/core/http_request.h"
#include "http/parser/http_parser_v2.h"

#include <utility>
#include <iostream>
#include <filesystem>
#include <sahara/sahara.h>
#include "http/exception/protocol_exception.h"
#include "http/core/http_block_data.h"
namespace obelisk::http {
    http_request::http_request(std::shared_ptr<http_block_data> &data) : data_(std::move(data)) {
        path_ = data_->header_.meta_.p2_;
        if (path_.contains("?")) {
            auto position = path_.find_first_of('?');
            //
            // std::string_view url_params = path_.substr(position +1);
            // parser::parse_urlencoded_param(*this, url_params);
            // path_ = std::string_view(path_.data(), position);
        }
        if(!data_->header_.headers_.contains("Content-Type") || data_->content_length_ <= 0)
            return;

        content_type_ = data_->header_.headers_["Content-Type"];
        parser::parse_body(*this);
    }
    void http_request::set_param(const std::string& name, const std::string& value) {
        if(params_.contains(name)){
            params_[name].push_back(value);
        }else{
            params_[name] = std::vector<std::string>{value};
        }
    }

    bool http_request::contains_param(const std::string &name) const {
        return params_.contains(name);
    }

    bool http_request::contains_file(const std::string &file) const {
        return filebag_.contains(file);
    }

    std::shared_ptr<http_file>& http_request::file(const std::string &file) {
        return filebag_[file];
    }

    const std::vector<std::string>& http_request::param(const std::string &name) {
        return params_[name];
    }

    const std::string& http_request::method() const{
        return data_->header_.meta_.p1_;
    }

    const std::string& http_request::target() const {
        return data_->header_.meta_.p2_;
    }

    const std::string& http_request::protocol() const{
        return data_->header_.meta_.p3_;
    }

    bool http_request::has_header(const std::string &name) const {
        return data_->header_.headers_.contains(name);
    }

    const std::string &http_request::header(const std::string &name) const{
        return data_->header_.headers_[name];
    }

    const std::string_view& http_request::content_type() {
        return content_type_;
    }

    http_request_wrapper::http_request_wrapper(boost::asio::io_context& ioctx, http_header& header, std::unique_ptr<std::iostream> raw_body) : raw_header_(std::move(header)), raw_body_(raw_body? std::move(raw_body):nullptr), ioctx_(ioctx) {
        const auto pos = raw_header_.meta_.p2_.find('?');
        target_ = std::string_view(raw_header_.meta_.p2_.data(),pos == -1? raw_header_.meta_.p2_.size(): pos);
    }

    http_request_wrapper::~http_request_wrapper() = default;

    boost::cobalt::task<void> http_request_wrapper::validate(const std::vector<validator::validator_group>& validators) {
        for (auto &[name, validator]: validators) {
            for (auto &j: validator) {
                co_await j->validate(name, *this);
            }
        }
        co_return;
    }

    sahara::container::unordered_smap_u<std::string>& http_request_wrapper::headers() {
        return raw_header_.headers_;
    }

    std::string_view http_request_wrapper::version() const {
        return raw_header_.meta_.p3_;
    }

    void http_request_wrapper::set_version(const std::string& version) {
        raw_header_.meta_.p3_ = version;
    }

    std::string_view http_request_wrapper::path() const {
        return raw_header_.meta_.p2_;
    }

    std::string_view http_request_wrapper::target() const {
        return target_;
    }

    std::string_view http_request_wrapper::method() const {
        return raw_header_.meta_.p1_;
    }

    std::string_view http_request_wrapper::query_string() const {
        const auto pos = raw_header_.meta_.p2_.find('?');

        return (pos== -1? std::string_view{}:std::string_view{raw_header_.meta_.p2_.data() + pos +1, raw_header_.meta_.p2_.size() - pos});
    }

    std::shared_ptr<std::iostream>& http_request_wrapper::raw_body() {
        return raw_body_;
    }

    std::unordered_map<std::string, std::any>& http_request_wrapper::registered_data() {
        return registered_value_;
    }

    std::unordered_map<std::string, boost::json::value>& http_request_wrapper::params() {
        return request_params_;
    }

    std::unordered_map<std::string, std::shared_ptr<http_file>>& http_request_wrapper::filebag() {
        return filebag_;
    }

    std::uint64_t http_request::content_length() const {
        return content_length_;
    }

    const std::string_view &http_request::path() const {
        return path_;
    }

    std::shared_ptr<http_block_data>& http_request::raw() {
        return data_;
    }

    http_temp_fstream::http_temp_fstream(std::string path): path_(std::move(path)) {
        std::ofstream fstream(path_);
        fstream.close();
        this->open(path_, std::ios::out | std::ios::in | std::ios::binary);
    }

    http_temp_fstream::~http_temp_fstream() {
        this->close();
        if(std::filesystem::exists(path_) && std::filesystem::is_regular_file(path_))
            std::filesystem::remove(path_);
    }

    std::string http_temp_fstream::path() {
        return path_;
    }
} // obelisk