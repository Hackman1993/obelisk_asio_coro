/***********************************************************************************************************************
 * @author Hackman Lo
 * @file route_item.cpp
 * @description 
 * @created_at 2023-10-09
***********************************************************************************************************************/
#include <vector>
#include "http/router/route_item.h"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include "http/exception/route_exception.h"
#include "http/router/route_param.h"
#include <regex>
#include <iostream>
BOOST_FUSION_ADAPT_STRUCT(obelisk::http::route_param, name_, static_);

namespace obelisk::http {
    using namespace boost::spirit::x3;
    auto route_param_parser = rule<class route_parser, route_param>{"RouteParamParser"} =
                                      (+~char_("*{}") >> attr(true) | '{' > +~char_("}") > '}' >> attr(false) | boost::spirit::x3::string("*") >> attr(false));

    route_item::route_item(const std::string &path, const std::function<std::shared_ptr<http_response>(std::shared_ptr<http_request> &)> &handler): handler_(handler){
        auto parse_result = boost::spirit::x3::parse(path.begin(), path.end(),*route_param_parser, pattern_);
        if(!parse_result)
            THROW(route_exception, "Invalid Route: " + path, "Obelisk");
        std::stringstream sstream("^");
        for(const auto& pattern: pattern_){
            if(pattern.static_){
                sstream << "(" << pattern.name_ << ")";
            } else {
                if(pattern.name_ == "*"){
                    sstream << "(.*)";
                }
                else{
                    sstream << "([^/]*)";
                }
            }
        }
        address_ = sstream.str();
    }

    bool route_item::match(const std::string &path, std::unordered_map<std::string,std::string>& route_params) {
        return route_param::extract(address_, path, pattern_, route_params);
    }

    bool route_item::method_allowed(const std::string &method) {
        return available_method_.contains(method);
    }

    route_item &route_item::method(const std::vector<std::string>& methods) {
        for(const auto& method: methods){
            available_method_.emplace(method, true);
        }
        return *this;
    }

    std::shared_ptr<http_response> route_item::handle(std::shared_ptr<http_request> &request) {
        if(handler_)
            return handler_(request);
        return nullptr;
    }
} // obelisk