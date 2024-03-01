#include "http/parser/http_parser_v2.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/algorithm/string.hpp>

#include <sahara/utils/uuid.h>
#include "http/exception/protocol_exception.h"
#include "http/core/http_block_data.h"
#include <boost/asio.hpp>
#include <iostream>
#include <filesystem>
#include "http/core/http_request.h"
#include <utility>
#include <boost/fusion/support/config.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
BOOST_FUSION_ADAPT_TPL_STRUCT((T1)(T2),(std::pair)(T1)(T2),(T1, first)(T2, second))
using namespace boost::spirit::x3;
BOOST_FUSION_ADAPT_STRUCT(obelisk::http::request_meta, p1_, p2_, p3_, is_req_)
BOOST_FUSION_ADAPT_STRUCT(obelisk::http::http_header, meta_, headers_)

namespace obelisk::http {
    #define RULE(name, attr) const auto name = rule<class mat_parser, attr>{#name}
    RULE(ContentTypeParser, std::string) = no_case["Content-Type"] > ":" > lexeme[*~char_("\r\n")];

    using string_pair = std::pair<std::string, std::string>;
    RULE(ServerUrlParser, std::string) = (char_("/") >> *~char_(" \r\n"));
    RULE(MetaParser, request_meta) = ((+~char_(" \r\n") >> " " >> ServerUrlParser >> " " >> +~char_(" \r\n") >> "\r\n" >> attr(true)) |
                                     (+~char_(" \r\n") >> " " >> +~char_(" \r\n") >> " " >> +~char_(" \r\n") >> "\r\n" >> attr(false)));
    RULE(HttpHeadersParser, string_pair) =  +~char_(":\r\n") >> ": " >> (+~char_("\r\n")) >> "\r\n";
    RULE(HttpPackageHeaderParser, http_header) = MetaParser >> +(HttpHeadersParser) >> "\r\n";
    RULE(MultipartBoundaryParser, std::string) = no_case["multipart/form-data; boundary="] >> (+~char_("\r\n"));

    RULE(MultipartMetaName, string_pair) = (no_case["name"] > attr(std::string("name")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
    RULE(MultipartMetaFilename, string_pair) = (no_case["filename"] > attr(std::string("filename")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
    RULE(MultipartMetaFormData, string_pair) = lit("form-data") > attr("form-data") > attr("form-data");
    RULE(MultipartMeta, string_pair) = MultipartMetaName | MultipartMetaFilename | MultipartMetaFormData;

    RULE(UrlEncodedData, string_pair) = *~char_("=&") >> -lit("=") >> *~char_("&");

    bool parser::parse_http_header(std::string_view data, obelisk::http::http_header &header) {
        auto result = parse(data.begin(), data.end(), HttpPackageHeaderParser, header);
        return result;
    }

    bool parser::parse_boundary(std::string_view data, std::string &boundary) {
        auto result = parse(data.begin(), data.end(), MultipartBoundaryParser, boundary);
        if(!result) {
            THROW(protocol_exception, "Boundary Parse Failed", "Obelisk");
        }
        return result;
    }

    bool parser::parse_body(obelisk::http::http_request &request) {
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

    bool parser::parse_multipart_body(obelisk::http::http_request_wrapper &request, const std::string& boundary) {
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
            bool parse_result = phrase_parse(line_data.begin(), line_data.end(), lit("Content-Disposition") > ':' > (MultipartMeta % ';'), space, meta_data);
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
                parse_result = parse(line_data.begin(), line_data.end(), ContentTypeParser, content_type);
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
                    file_buffer = std::string_view(boost::asio::buffer_cast<const char *>(sbuf_.data()), sbuf_.size());
                    if (!file_buffer.contains(boundary_split_data)) {
                        fstream->write(file_buffer.data(), file_buffer.size() - boundary.size() - 5);
                        sbuf_.consume(file_buffer.size() - boundary.size() - 5);
                    } else {
                        auto range = boost::algorithm::find_first(file_buffer, "\r\n--" + boundary);
                        std::string_view file_content_part = std::string_view(file_buffer.data(), range.begin() - file_buffer.begin());
                        fstream->write(file_content_part.data(), static_cast<std::streamsize>(file_content_part.size()));
                        sbuf_.consume(file_content_part.size() + 2);
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
                request.params()[meta_data["name"]].push_back(line_data);
            }
        }
        return true;
    }

    bool parser::parse_urlencoded_param(http_request_wrapper &request, std::string_view data) {

        std::vector<std::pair<std::string, std::string>> params;
        if (!parse(data.begin(), data.end(), UrlEncodedData % '&', params))
            throw protocol_exception("UrlEncodedData Parse Failed!");
        for (auto &[key, val]: params) {
            if(!request.request_params_.contains(key))
                request.request_params_[key] = std::vector<std::string>();
            request.request_params_[key].emplace_back(val);
        }
        return true;
    }

} // obelisk