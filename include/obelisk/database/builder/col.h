//
// Created by hackman on 4/27/25.
//

#ifndef COL_H
#define COL_H
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <obelisk/http/framework/config.h>

#include "base_statement.h"

class col : public base_statement
{
public:
    explicit col(std::string str): col_(std::move(str)){ }
    col(const char* str): col(std::string(str)){}
    col(std::string name, std::string alias): col_(std::move(name)), alia_(std::move(alias)) { }
    col(const char* name, const char* alia): col(std::string(name), std::string(alia)){}


    std::string compile() override
    {
        auto prefix = obelisk::http::config::get<std::string>("database.default.prefix", "");
        bool contains_table_name = boost::algorithm::contains(col_, ".");
        auto column_name = col_;
        if (contains_table_name)
            boost::algorithm::replace_all(column_name, ".", "`.`");
        std::string result = std::format("`{}{}`{}", contains_table_name? prefix:"", column_name, alia_? " AS " + alia_.value(): "");
        return result;
    }

private:
    std::string col_;
    std::optional<std::string> alia_;
};

#endif //COL_H
