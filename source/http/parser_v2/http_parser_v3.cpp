#include "obelisk/http/parser/http_parser_v3.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/algorithm/string.hpp>

#include <sahara/utils/uuid.h>
#include "obelisk/http/exception/protocol_exception.h"
#include "obelisk/http/core/raw.h"
#include <boost/asio.hpp>
#include <iostream>
#include <filesystem>
#include "obelisk/http/core/http_request.h"
#include <utility>
#include <nlohmann/json.hpp>
#include <boost/parser/parser.hpp>
using namespace boost::parser;

namespace obelisk::http {
    auto ContentTypeParser= no_case[lit("Content-Type")] > ":" > lexeme[*char_-"\r\n"];
    rule<struct MetaParser, core::raw::http_meta_raw> MetaParser= "MetaParser";
    auto MetaParser_def = +(char_ - char_(" \r\n")) > *ws > +(char_ - char_(" \r\n")) > *ws> +(char_ - char_("\r\n")) > *lit(" ") > "\r\n";
    BOOST_PARSER_DEFINE_RULES(MetaParser);
    rule<struct HttpHeaderParser, std::pair<std::string, std::string>> HttpHeaderParser= "HttpHeaderParser";
    auto HttpHeaderParser_def = +(char_ - char_(" :\r\n"))> *lit(" ") > ':' > *ws> +(char_- "\r\n")> "\r\n";
    BOOST_PARSER_DEFINE_RULES(HttpHeaderParser);

    rule<struct HttpHeadersParser, std::vector<std::pair<std::string, std::string>>> HttpHeadersParser=  "HttpHeadersParser";
    auto HttpHeadersParser_def = +(HttpHeaderParser);
    BOOST_PARSER_DEFINE_RULES(HttpHeadersParser);

    rule<struct HttpPackageHeaderParser, core::raw::http_header_raw> HttpPackageHeaderParser=  "HttpPackageHeaderParser";
    auto HttpPackageHeaderParser_def = MetaParser > HttpHeadersParser > "\r\n";
    BOOST_PARSER_DEFINE_RULES(HttpPackageHeaderParser);

    rule<struct MultipartMetaName, std::pair<std::string, std::string>> MultipartMetaNameParser= "MultipartMetaName";
    rule<struct MultipartMetaFileName, std::pair<std::string, std::string>> MultipartMetaFileNameParser= "MultipartMetaFileName";
    rule<struct MultipartMetaFormData, std::pair<std::string, std::string>> MultipartMetaFormDataParser= "MultipartMetaFormData";
    rule<struct MultipartMeta, std::pair<std::string, std::string>> MultipartMetaParser= "MultipartMeta";
    auto MultipartMetaNameParser_def = (no_case[lit("name")] > attr(std::string("name")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
    auto MultipartMetaFileNameParser_def = (no_case[lit("filename")] > attr(std::string("filename")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
    auto MultipartMetaFormDataParser_def = lit("form-data") > attr("form-data") > attr("form-data");
    auto MultipartMetaParser_def = MultipartMetaNameParser | MultipartMetaFileNameParser | MultipartMetaFormDataParser;

    BOOST_PARSER_DEFINE_RULES(MultipartMetaNameParser, MultipartMetaFileNameParser, MultipartMetaFormDataParser, MultipartMetaParser)
    auto MultipartBoundaryParser = no_case[lit("multipart/form-data;")] > *ws > no_case[lit("boundary=")] > *ws >> +(char_ - char_("\r\n")) > *ws;
    rule<struct UrlEncodedDataName, std::pair<std::string, std::string>> UrlEncodedDataParser= "UrlEncodedDataName";
    auto UrlEncodedDataParser_def = *(char_ - char_("=&")) >> -lit("=") >> *(char_ - char_("&"));
    BOOST_PARSER_DEFINE_RULES(UrlEncodedDataParser);

    //auto ContentTypeParser= no_case[lit("Content-Type")] > ":" > lexeme[*char_-"\r\n"];
    // auto ServerUrlParser = lexeme[(char_("/") > *(char_-char_(" \r\n")))];
    // rule<struct MetaParser, obelisk::http::request_meta> MetaParser= "MetaParser";
    // auto MetaParser_def = ((+char_ - char_(" \r\n")) >>  ServerUrlParser >> +(char_ - char_(" \r\n"))  >> attr(true)) |
    //                                   ((+char_ - char_(" \r\n")) >> +(char_- char_(" \r\n"))  >> +(char_ - char_(" \r\n")) >> attr(false));
    // BOOST_PARSER_DEFINE_RULES(MetaParser);
    //
    // rule<struct HttpHeaderParser, std::pair<std::string, std::string>> HttpHeaderParser= "HttpHeaderParser";
    // auto HttpHeaderParser_def = +(char_ - char_(" :\r\n"))> *lit(" ") > ':' > *ws> (+char_- "\r\n");
    // BOOST_PARSER_DEFINE_RULES(HttpHeaderParser);
    //
    // rule<struct HttpHeadersParser, http_header> HttpHeadersParser= "HttpHeadersParser";
    // auto HttpHeadersParser_def = MetaParser > +(HttpHeadersParser) > "\r\n";

    // using string_pair = std::pair<std::string, std::string>;
    // auto HttpPackageHeaderParser = MetaParser >> +(HttpHeadersParser) >> "\r\n";
    // RULE(MultipartBoundaryParser, std::string) = no_case["multipart/form-data; boundary="] >> (+~char_("\r\n"));

    // RULE(MultipartMetaName, string_pair) = (no_case["name"] > attr(std::string("name")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
    // RULE(MultipartMetaFilename, string_pair) = (no_case["filename"] > attr(std::string("filename")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
    // RULE(MultipartMetaFormData, string_pair) = lit("form-data") > attr("form-data") > attr("form-data");
    // RULE(MultipartMeta, string_pair) = MultipartMetaName | MultipartMetaFilename | MultipartMetaFormData;
    //
    // RULE(UrlEncodedData, string_pair) = *~char_("=&") >> -lit("=") >> *~char_("&");
    // RULE(UrlPartsData, url_parts) = *((string("https")|string("http")) > lit("://")) > (+~char_("/?")) >> *(+~char_("?")) >> *(lit("?") >> (*char_));

    bool parser_v3::parse_http_header(std::string_view data, obelisk::http::core::raw::http_header_raw &header) {
        auto result = parse(data, HttpPackageHeaderParser, header);
        return result;
    }

    bool parser_v3::parse_boundary(std::string_view data, std::string &boundary) {
        auto result = parse(data, MultipartBoundaryParser, boundary);
        if(!result) {
            THROW(protocol_exception, "Boundary Parse Failed", "Obelisk");
        }
        return result;
    }

    bool parser_v3::parse_body(http_request &request) {
        // auto content_type = request.content_type();
        // auto content_length = request.content_length();
        // if (boost::algorithm::icontains(content_type, "multipart/form-data")) {
        //     std::string boundary;
        //     parse_boundary(content_type, boundary);
        //     return parse_multipart_body(request, boundary);
        // } else if (boost::algorithm::icontains(content_type, "application/x-www-form-urlencoded")) {
        //     if (request.content_length() > 1 * 1024 * 1024) {
        //         THROW(protocol_exception, "UrlEncodedBody Exceeds Maximum Size Limit (1MB)", "Obelisk");
        //     }
        //     std::string data_;
        //     data_.reserve(request.content_length());
        //     auto data = request.raw();
        //     data->content_->seekg(std::ios::beg);
        //     data->content_->read(data_.data(), (std::int64_t)content_length);
        //     auto bytes_read = data->content_->gcount();
        //     if (bytes_read != content_length) {
        //         THROW(protocol_exception, "Content-Length not match stream size", "Obelisk");
        //     }
        //     return parse_urlencoded_param(request, data_);
        // }
        return true;
    }

    bool parser_v3::parse_multipart_body(obelisk::http::http_request_wrapper &request, const std::string& boundary) {
        std::string boundary_end_data = "--" + boundary + "--\r";
        std::string boundary_split_data = "--" + boundary;
        auto data = request.raw_body();
        data->seekg(std::ios::beg);
        while (!data->eof()) {
            // Read Boundary
            std::string line_data;
            std::getline(*data, line_data);
            if (line_data == boundary_end_data) return true;
            if (line_data.empty()) {
                THROW(protocol_exception, "Invalid Multipart form data_", "Obelisk");
            }

            // Read Meta Data
            bool is_file = false;
            std::string content_type, temp_file_path;
            std::unique_ptr<std::ofstream> fstream;

            std::getline(*data, line_data);
            std::unordered_map<std::string, std::string> meta_data;
            bool parse_result = parse(line_data, lit("Content-Disposition") > ':' > (MultipartMetaParser % ';'), ws, meta_data);
            if (!parse_result)
                THROW(protocol_exception, "Multipart form data_ meta parse failed!", "Obelisk");
            if (meta_data.contains("filename")) {
                is_file = true;
                temp_file_path = "./" + sahara::utils::uuid::generate() + std::filesystem::path(meta_data["filename"]).extension().string();
                fstream = std::make_unique<std::ofstream>(temp_file_path, std::ios::binary);
            }

            // If content is a file, should read content type
            if (is_file) {
                std::getline(*data, line_data);
                if (line_data.empty() || line_data == "\r")
                    THROW(protocol_exception, "Form data was a file but no content type was specified", "Obelisk");
                parse_result = parse(line_data, ContentTypeParser, content_type);
                if (!parse_result) {
                    THROW(protocol_exception, "Multipart form data_ Content-Type parse failed!", "Obelisk");
                }
            }
            std::getline(*data, line_data);
            if (line_data != "\r") {
                THROW(protocol_exception, "Unexpected Line Data", "Obelisk");
            }

            // If not a file, read data_
            if (!is_file) {
                std::getline(*data, line_data);
                if (line_data.empty()) {
                    THROW(protocol_exception, "Invalid Multipart Form Data", "Obelisk");
                }
                line_data.resize(line_data.length() - 1);
            } else {
                char data_[10240] = {0};
                boost::asio::streambuf sbuf_;
                sbuf_.prepare(10240);
                std::string_view file_buffer;

                do {
                    memset(data_, 0, 10240);
                    data->read(data_, 10240);
                    auto bytes_read = data->gcount();
                    sbuf_.sputn(data_, bytes_read);
                    file_buffer = std::string_view(static_cast<const char *>(sbuf_.data().data()), sbuf_.size());
                    if (!file_buffer.contains(boundary_split_data)) {
                        fstream->write(file_buffer.data(), file_buffer.size() - boundary.size() - 5);
                        sbuf_.consume(file_buffer.size() - boundary.size() - 5);

                    } else {
                        auto range = boost::algorithm::find_first(file_buffer, "\r\n--" + boundary);
                        std::string_view file_content_part = std::string_view(file_buffer.data(), range.begin() - file_buffer.begin());
                        fstream->write(file_content_part.data(), static_cast<std::streamsize>(file_content_part.size()));
                        sbuf_.consume(file_content_part.size() + 2);
                        // Prevent read too much data
                        data->clear();
                        data->seekg(int(-sbuf_.size()), std::ios::cur);
                    }
                } while (!file_buffer.contains(boundary_split_data));
                fstream->flush();
                fstream->close();
            }
            if (!meta_data.contains("name"))
                continue;
            if (is_file && meta_data.contains("filename")) {
                request.filebag_[meta_data["name"]] = std::make_shared<http_file>(temp_file_path, meta_data["filename"]);
            } else {
                auto item_key = boost::algorithm::replace_last_copy(meta_data["name"], "[]", "");
                bool is_array = request.request_params_.contains(item_key)? request.request_params_[item_key].is_array() : false;
                if(request.request_params_.contains(item_key) && !request.request_params_[item_key].is_array()) {
                    nlohmann::json new_value = { request.request_params_[item_key]};
                    request.request_params_[item_key] = new_value;
                }
                if(is_array) {
                    request.request_params_[meta_data["name"]].emplace_back(line_data);
                } else {
                    request.params()[meta_data["name"]] = nlohmann::json(line_data);
                }
            }
        }
        return true;
    }

    bool parser_v3::parse_urlencoded_param(std::unordered_map<std::string, nlohmann::json>& params, std::string_view data) {
        if (!parse(data, UrlEncodedDataParser % '&', params))
            throw protocol_exception("UrlEncodedData Parse Failed!");

        for (auto &[key, val]: params) {
            auto item_key = key.substr(0, key.rfind("[]"));
            bool is_array = key.ends_with("[]");
            if(is_array && !params.contains(key)) {
                params[key] = nlohmann::json::array();
            }
            if(is_array) {
                params[key].emplace_back(val);
            } else {
                params[key] = val;
            }
        }
        return true;
    }

    std::unique_ptr<url_parts> parser_v3::parse_split_url(const std::string &uri) {
        // std::unique_ptr<url_parts> ptr = std::make_unique<url_parts>();
        //
        // auto result = parse(uri.begin(), uri.end(), UrlPartsData, *ptr);
        // if(!result) {
        //     THROW(protocol_exception, "Invalid Url String", "Obelisk");
        //     return nullptr;
        // }
        // if(ptr->path.empty())
        //     ptr->path = "/";
        // return ptr;
        return nullptr;
    }

} // obelisk