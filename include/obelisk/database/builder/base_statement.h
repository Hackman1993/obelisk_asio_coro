//
// Created by hackman on 4/25/25.
//

#ifndef BASE_STATEMENT_H
#define BASE_STATEMENT_H
#include <chrono>
#include <string>
#include <variant>
using sql_value_t = std::variant<std::nullptr_t, double, bool, std::int64_t, std::string, std::uint64_t, std::chrono::system_clock::time_point>;

struct value_compile_visitor {
    std::string operator()(const std::string& s) const {
        return std::format("'{}'", s);
    }

    std::string operator()(std::nullptr_t) const
    {
        return "NULL";
    }
    std::string operator()(const bool b) const {
        return b ? "1" : "0";
    }

    std::string operator()(const std::chrono::system_clock::time_point& tp) const
    {
        return std::format("'{:%F %T}'", tp);
    }
    template <typename T>
    std::string operator()(const T i) const {
        return std::to_string(i);
    }
};

class base_statement
{
public:
    virtual ~base_statement() = default;

    virtual std::string compile() = 0;
};

class sql_value : public sql_value_t
{
public:
    using sql_value_t::variant;
    std::string compile()
    {
        return std::visit(value_compile_visitor{}, *this);
    }
};
#endif //BASE_STATEMENT_H
