//
// Created by hackman on 4/25/25.
//

#ifndef CONDITION_H
#define CONDITION_H
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <utility>
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

    template<typename T>
    condition_operands(const T& value): var_(sql_value(value)){}
    std::string compile() override
    {
        return std::visit(condition_operand_visitor{}, var_);
    }
    condition_operands(col value): var_(std::move(value)){}

    [[nodiscard]] bool is_null() const
    {
        return var_.index() == 1 && std::get<sql_value>(var_).index() == 0;
    }

private:
    condition_operand_t var_;
};

class condition : public base_statement {
public:
    condition(condition_operands  first, condition_operands  second): first_(std::move(first)), second_(std::move(second)){}
    condition(condition_operands first, std::string operator_string, condition_operands  second): first_(std::move(first)), second_(std::move(second)), operator_(std::move(operator_string)){}
    // condition(const std::string& column, std::string operator_string, sql_value value): first_(col(column)), second_(value), operator_(std::move(operator_string)){}
    // condition(const char* col, const char* operator_string, const sql_value& value) : condition(std::string(col), operator_string, value){}
    // condition(const std::string& column): first_(col(column)), second_(sql_value(nullptr)), operator_("IS NOT"){}
    // condition(const char* column): first_(col(column)), second_(sql_value(nullptr)), operator_("IS NOT"){}
    // condition(const char* column, const sql_value& value): condition(column, " = ", value){}

    std::string compile() override
    {
        return std::format("{} {} {}", first_.compile(), second_.is_null()? "IS":operator_, second_.compile());
    };
private:
    condition_operands first_, second_;
    std::string operator_ = " = ";
};
#endif //CONDITION_H
