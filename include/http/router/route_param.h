/***********************************************************************************************************************
 * @author Hackman Lo
 * @file route_param.h
 * @description 
 * @created_at 2023-10-09
***********************************************************************************************************************/

#ifndef OBELISK_ROUTE_PARAM_H
#define OBELISK_ROUTE_PARAM_H
#include <string>
#include <regex>
#include <unordered_map>
namespace obelisk::http {

    struct route_param {
        std::string name_;
        bool static_ = false;

        static bool extract(const std::regex& regex, const std::string& path, const std::vector<route_param>& pattern, std::unordered_map<std::string, std::string>& params);
    };
} // obelisk
#endif //OBELISK_ROUTE_PARAM_H
