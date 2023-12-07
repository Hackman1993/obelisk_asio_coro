/***********************************************************************************************************************
 * @author Hackman Lo
 * @file route_param.cpp
 * @description 
 * @created_at 2023-10-09
***********************************************************************************************************************/

#include "http/router/route_param.h"

namespace obelisk::http {

    bool route_param::extract(const std::regex &regex, const std::string &path, const std::vector<route_param> &pattern, std::unordered_map<std::string, std::string> &params) {
        std::smatch smatch;
        if(std::regex_match(path, smatch,regex)) {
            if(smatch.size()-1 != pattern.size())
                return false;
            for(int i = 1; i < smatch.size(); ++i){
                auto data = smatch[i].str();
                if(pattern[i-1].static_ || pattern[i-1].name_ =="*")
                    continue;
                else{
                    params.emplace(pattern[i-1].name_, data);
                }
            }
            return true;
        }
        return false;
    }
} // obelisk