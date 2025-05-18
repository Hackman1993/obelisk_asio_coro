//
// Created by hackman on 5/16/25.
//

#ifndef COMMON_H
#define COMMON_H
#include <chrono>
#include <string>
#include <variant>

namespace obelisk::database::core
{
    using sql_value_t = std::variant<std::nullptr_t, double, bool, std::int64_t, std::string, std::uint64_t, std::chrono::system_clock::time_point>;
    class base_statement
    {
    public:
        virtual ~base_statement() = default;
        virtual std::string compile() = 0;
    };

    struct general_visitor {
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<base_statement, T>>>
        std::string operator()(T& c) const {
            return c.compile();
        }
        std::string operator()(std::string& c) const {
            return std::format("'{}'", c);
        }
        std::string operator()(sql_value_t& c) const {
            return std::visit(general_visitor{}, c);
        }
        std::string operator()(std::chrono::system_clock::time_point& c) const
        {
            return std::format("'{:%F %T}'", c);
        }
        template <typename T, typename=std::enable_if_t<!std::is_class_v<T>>>
        std::string operator()(T c) const {
            return std::format("{}", c);
        }
        std::string operator()(std::nullptr_t) const {
            return "NULL";
        }
    };

    class sql_value : public sql_value_t, base_statement
    {
    public:
        using sql_value_t::variant;
        std::string compile() override
        {
            return std::visit(general_visitor{}, *this);
        }
    };
}

#endif //COMMON_H
