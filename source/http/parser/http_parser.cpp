////
//// Created by hackman on 10/7/23.
////
//
//#include "http/parser/http_parser.h"
//#include <boost/spirit/home/x3.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/fusion/adapted/std_pair.hpp>
//#include <sahara/utils/uuid.h>
//#include "http/exception/protocol_exception.h"
//#include <boost/asio.hpp>
//#include <iostream>
//#include <filesystem>
//
//using namespace boost::spirit::x3;
//BOOST_FUSION_ADAPT_STRUCT(obelisk::http::request_title, method_, target_, protocol_)
//BOOST_FUSION_ADAPT_STRUCT(obelisk::http::http_header, title_, headers_)
//namespace obelisk::http {
//    auto content_type_parser = rule<class mat_parser, std::string>{"ContentTypeParser"} = no_case["Content-Type"] > ":" > lexeme[*~char_("\r\n")];
//
//    auto mat_parser_ = rule<class mat_parser, request_title>{"MATParser"} = +~char_(" \r\n") >> " " >> +~char_(" \r\n") >> " " >> no_case["HTTP/"] >> +~char_(" \r\n") >> "\r\n";
//    auto http_headers_parser_ = rule<class mat_parser, std::pair<std::string, std::string> >{"HttpHeadersParser"} = +~char_(":\r\n") >> ": " >> (+~char_("\r\n")) >> "\r\n";
//    auto http_package_header_parser_ = rule<class mat_parser, http_header>{"HttpPackageHeaderParser"} = mat_parser_ >> +(http_headers_parser_) >> "\r\n";
//    auto boundary_parser_ = no_case["multipart/form-data_; boundary="] >> (+~char_("\r\n"));
//
//    auto multipart_meta_name = rule<class mat_parser, std::pair<std::string, std::string>>{"MultipartName"} = (no_case["name"] > attr(std::string("name")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
//    auto multipart_meta_filename = rule<class mat_parser, std::pair<std::string, std::string>>{"MultipartFileName"} = (no_case["filename"] > attr(std::string("filename")) > "=" > '"' > lexeme[*(char_ - '"')] > '"');
//    auto multipart_meta_formdata = rule<class mat_parser, std::pair<std::string, std::string>>{"MultipartFormData"} = lit("form-data_") > attr("form-data_") > attr("form-data_");
//    auto multipart_meta = rule<class mat_parser, std::pair<std::string, std::string>>{"MultipartMeta"} = multipart_meta_name | multipart_meta_filename | multipart_meta_formdata;
//
//    auto http_urlencoded_parser = rule<class mat_parser, std::pair<std::string, std::string>>{"HttpUrlEncodedParser"} = *~char_("=&") >> -lit("=") >> *~char_("&");
//
//    bool package_header_parse(std::string_view data_, std::shared_ptr<http_request> request) {
//        auto result = parse(data_.begin(), data_.end(), http_package_header_parser_, request->header_);
//        const std::string &target_raw = request->header_.title_.target_;
//        if (result && target_raw.contains("?")) {
//            request->path_ = target_raw.substr(0, target_raw.find_first_of('?'));
//            std::string url_params = target_raw.substr(target_raw.find_first_of('?') + 1);
//            http_urlencoded_param_parser(request, url_params);
//        }else{
//            request->path_ = target_raw;
//        }
//        return result;
//    }
//
//    bool boundary_parse(std::string_view data_, const std::shared_ptr<http_request> &request) {
//        auto result = parse(data_.begin(), data_.end(), boundary_parser_, request->boundary_);
//        return result;
//    }
//
//    bool http_body_parser(std::shared_ptr<http_request> &request) {
//        if (boost::algorithm::icontains(request->content_type_, "multipart/form-data_")) {
//            return http_multipart_body_parser(request);
//        } else if (boost::algorithm::icontains(request->content_type_, "application/x-www-form-urlencoded")) {
//            if (request->content_length_ > 1 * 1024 * 1024) {
//                THROW(protocol_exception, "UrlEncodedBody Exceeds Maximum Size Limit (1MB)", "Obelisk");
//            }
//            std::string data_;
//            data_.reserve(request->content_length_);
//            request->raw_->seekg(std::ios::beg);
//            request->raw_->read(data_.data_(), request->content_length_);
//            auto bytes_read = request->raw_->gcount();
//            if (bytes_read != request->content_length_) {
//                THROW(protocol_exception, "Content-Length not match stream size", "Obelisk");
//            }
//            return http_urlencoded_param_parser(request, data_);
//        }
//        return true;
//    }
//
//    bool http_multipart_body_parser(std::shared_ptr<http_request> &request) {
//        if (request->boundary_.empty())
//            THROW(protocol_exception, "Invalid Boundary", "Obelisk");
//        std::string boundary_end_data = "--" + request->boundary_ + "--\r";
//        std::string boundary_split_data = "--" + request->boundary_;
//        request->raw_->seekg(std::ios::beg);
//        while (!request->raw_->eof()) {
//            // Read Boundary
//            std::string line_data;
//            std::getline(*request->raw_, line_data);
//            if (line_data == boundary_end_data) return true;
//            if (line_data.empty()) {
//                THROW(protocol_exception, "Invalid Multipart form data_", "Obelisk");
//            }
//
//            // Read Meta Data
//            bool is_file = false;
//            std::string content_type, temp_file_path;
//            std::unique_ptr<std::ofstream> fstream;
//
//            std::getline(*request->raw_, line_data);
//            std::unordered_map<std::string, std::string> meta_data;
//            bool parse_result = phrase_parse(line_data.begin(), line_data.end(), lit("Content-Disposition") > ':' > (multipart_meta % ';'), space, meta_data);
//            if (!parse_result)
//                THROW(protocol_exception, "Multipart form data_ meta parse failed!", "Obelisk");
//            if (meta_data.contains("filename")) {
//                is_file = true;
//                temp_file_path = "./" + sahara::utils::uuid::generate() + std::filesystem::path(meta_data["filename"]).extension().string();
//                fstream = std::make_unique<std::ofstream>(temp_file_path, std::ios::binary);
//            }
//
//            // If content is a file, should read content type
//            if (is_file) {
//                std::getline(*request->raw_, line_data);
//                if (line_data.empty() || line_data == "\r")
//                    THROW(protocol_exception, "Form data_ was a file but no content type was specified", "Obelisk");
//                parse_result = parse(line_data.begin(), line_data.end(), content_type_parser, content_type);
//                if (!parse_result) {
//                    THROW(protocol_exception, "Multipart form data_ Content-Type parse failed!", "Obelisk");
//                }
//            }
//            std::getline(*request->raw_, line_data);
//            if (line_data != "\r") {
//                THROW(protocol_exception, "Unexpected Line Data", "Obelisk");
//            }
//
//            // If not a file, read data_
//            if (!is_file) {
//                std::getline(*request->raw_, line_data);
//                if (line_data.empty()) {
//                    THROW(protocol_exception, "Invalid Multipart Form Data", "Obelisk");
//                }
//                line_data.resize(line_data.length() - 1);
//            } else {
//                char data_[10240] = {0};
//                boost::asio::streambuf sbuf_;
//                sbuf_.prepare(10240);
//                std::string_view file_buffer;
//
//                do {
//                    memset(data_, 0, 10240);
//                    request->raw_->read(data_, 10240);
//                    auto bytes_read = request->raw_->gcount();
//                    sbuf_.sputn(data_, bytes_read);
//                    file_buffer = std::string_view(boost::asio::buffer_cast<const char *>(sbuf_.data_()), sbuf_.size());
//                    if (!file_buffer.contains(boundary_split_data)) {
//                        fstream->write(file_buffer.data_(), file_buffer.size() - request->boundary_.size() - 5);
//                        sbuf_.consume(file_buffer.size() - request->boundary_.size() - 5);
//                    } else {
//                        auto range = boost::algorithm::find_first(file_buffer, "\r\n--" + request->boundary_);
//                        std::string_view file_content_part = std::string_view(file_buffer.data_(), range.begin() - file_buffer.begin());
//                        fstream->write(file_content_part.data_(), static_cast<std::streamsize>(file_content_part.size()));
//                        sbuf_.consume(file_content_part.size() + 2);
//                    }
//                } while (!file_buffer.contains(boundary_split_data));
//                fstream->flush();
//                fstream->close();
//            }
//            if (!meta_data.contains("name"))
//                continue;
//            if (is_file && meta_data.contains("filename")) {
//                request->filebag_[meta_data["name"]] = std::make_shared<http_file>(temp_file_path, meta_data["filename"]);
//            } else {
//                request->set_param(meta_data["name"], line_data);
//            }
//        }
//        return true;
//    }
//
//    bool http_urlencoded_param_parser(std::shared_ptr<http_request> &request, std::string_view data_) {
//
//        std::vector<std::pair<std::string, std::string>> params;
//        bool parse_result = parse(data_.begin(), data_.end(), http_urlencoded_parser % '&', params);
//        if (!parse_result)
//            THROW(protocol_exception, "UrlEncodedData Parse Failed!", "Obelisk");
//        for (auto &item: params) {
//            request->set_param(item.first, item.second);
//        }
//        return true;
//    }
//
//} // obelisk