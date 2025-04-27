//
// Created by hackman on 4/27/25.
//

#ifndef TABLE_H
#define TABLE_H
#include <obelisk/http/framework/config.h>

#include "base_statement.h"

using table_type_t = std::variant<std::string>;

struct table_compile_visitor
{
    std::string operator()(const std::string& s) const {
        return std::format("`{}{}`", obelisk::http::config::get<std::string>("database.default.prefix", ""), s);
    }
};

class table : public base_statement
{
public:
    explicit table(std::string str): table_(std::move(str)){ }
    table(const char* str): table(std::string(str)){}
    table(std::string name, std::string alias): table_(std::move(name)), alia_(std::move(alias)) { }
    table(const char* name, const char* alia): table(std::string(name), std::string(alia)){}

    std::string compile() override
    {
        return std::visit(table_compile_visitor{}, table_);
    }

private:
    table_type_t table_;
    std::optional<std::string> alia_;
};
#endif //TABLE_H
