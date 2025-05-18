#ifndef OBELISK_DATABASE_BUILDER_DETAIL_ALIA_H
#define OBELISK_DATABASE_BUILDER_DETAIL_ALIA_H
#include "col.h"
#include "raw.h"
#include "table.h"

namespace obelisk::database::builder::detail
{
    struct alia_compile_visitor
    {
        template<typename T>
        requires std::is_base_of_v<core::base_statement, T>
        std::string operator()(T& c)
        {
            return c.compile();
        }
        template<typename T>
        requires (!(std::is_base_of_v<core::base_statement, T>))
        std::string operator()(T& c)
        {
            return c.compile();
        }
        std::string operator()(raw& c) const
        {
            return std::format("({})", c.compile());
        }
    };

    template <typename T, typename... Args>
    inline constexpr bool contains_type_v = std::disjunction_v<std::is_same<T, Args>...>;

    template <typename... Args>
    class alia :public core::base_statement
    {
    public:
        template<typename T>
        alia(T tar, std::string alia): source_(tar), alia_(alia){ }
        template<typename T>
        requires (std::is_base_of_v<base_statement, T> || !contains_type_v<core::sql_value, Args...>)
        alia(const T& tar): source_(std::move(tar)){ }
        // template<typename T>
        // requires (contains_type_v<core::sql_value, Args...> && !std::is_base_of_v<base_statement, T>)
        // alia(T tar): source_(core::sql_value(tar)){ }
        // template<typename T>
        // alia(const std::enable_if_t<std::is_base_of_v<base_statement, T>,T&> tar): source_(tar){ }
        template<typename T>
        requires (!std::is_base_of_v<base_statement, T> && contains_type_v<core::sql_value, Args...>)
        alia(const T& tar): source_(core::sql_value(tar)){ }

        std::string compile() override
        {
            std::string result = std::visit(alia_compile_visitor{}, source_);
            std::string prefix;
            if constexpr (contains_type_v<table, Args...>)
                prefix = http::config::get<std::string>("database.default.prefix", "");
            if (alia_)
                result.append(std::format(" AS `{}{}` ", prefix , alia_.value()));
            return result;
        }

    protected:
        std::variant<Args...> source_;
        std::optional<std::string> alia_;
    };
}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_ALIA_H
