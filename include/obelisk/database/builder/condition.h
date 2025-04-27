//
// Created by hackman on 4/25/25.
//

#ifndef CONDITION_H
#define CONDITION_H
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <obelisk/http/framework/config.h>

#include "condition.h"
#include "base_statement.h"
#include "col.h"
#include "raw.h"

using condition_operand_t = std::variant<col, sql_value>;

struct condition_operand_visitor
{
    template <typename T>
    std::string operator()(T& c) const {
        return c.compile();
    }
};

class condition_operands : public base_statement
{
public:
    explicit condition_operands(const std::variant<col, sql_value>& var) :var_(var){}

    std::string compile() override
    {
        return std::visit(condition_operand_visitor{}, var_);
    }

private:
    condition_operand_t var_;
};

class condition : public base_statement {
public:
    condition(const std::string& column, std::string operator_string, sql_value value): first(col(column)), second(value), operator_(std::move(operator_string)){}
    condition(const char* col, const char* operator_string, const sql_value& value) : condition(std::string(col), operator_string, value){}
    condition(const std::string& column): first(col(column)), second(sql_value(nullptr)){}
    condition(const char* column): first(col(column)), second(sql_value(nullptr)), operator_(" NOT "){}
    condition(const char* column, const sql_value& value): condition(column, " = ", value){}

    std::string compile() override
    {
        return std::format("{} {} {}", first.compile(), operator_, second.compile());
    };
private:
    condition_operands first, second;
    std::string operator_ = " = ";
};
#endif //CONDITION_H
