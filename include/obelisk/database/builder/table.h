//
// Created by hackman on 4/27/25.
//

#ifndef TABLE_H
#define TABLE_H
#include <obelisk/http/framework/config.h>

#include "base_statement.h"

// struct table_compile_visitor
// {
//     std::string operator()(const std::string& s) const {
//
//     }
// };

class table : public base_statement
{
public:
    explicit table(std::string str): table_(std::move(str)){ }
    table(const char* str): table(std::string(str)){}
    table(std::string name, std::string alias): table_(std::move(name)), alia_(std::move(alias)) { }
    table(const char* name, const char* alia): table(std::string(name), std::string(alia)){}

    std::string compile() override
    {
        auto prefix = obelisk::http::config::get<std::string>("database.default.prefix", "");
        std::string val = std::format("`{}{}`", prefix, table_);
        if (alia_)
            val.append(std::format(" AS {}{}", prefix, alia_.value()));
        return val;
    }

private:
    std::string table_;
    std::optional<std::string> alia_;
};
#endif //TABLE_H
