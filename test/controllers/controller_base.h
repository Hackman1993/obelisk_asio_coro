//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef CONTROLLER_BASE_H
#define CONTROLLER_BASE_H
#include <boost/cobalt.hpp>
#include <boost/mysql/results.hpp>

#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>
#include <boost/cobalt/task.hpp>
class mysql_connection;

class controller_base {
public:
    static std::unique_ptr<obelisk::http::http_response> json_response(const boost::mysql::results& result, const std::unordered_map<std::string, boost::json::value>& additional_field = {} , obelisk::http::EResponseCode = obelisk::http::EST_OK);
    static obelisk::task<std::string> save_attachment(obelisk::http::http_file& file, std::string path, std::shared_ptr<mysql_connection> connection, std::uint64_t uploader_id, const std::string& replace = "");
    static std::string escape_string(const std::string& str);
    static std::string escape_string(const std::optional<std::string>& str);

    static obelisk::task<std::unique_ptr<obelisk::http::http_response>> getTerminalToken(obelisk::http::http_request_wrapper&request);


};





#endif //CONTROLLER_BASE_H
