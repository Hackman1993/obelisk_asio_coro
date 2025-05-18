//
// Created by hackman on 4/25/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_CONDITION_H
#define OBELISK_DATABASE_BUILDER_DETAIL_CONDITION_H
#include <boost/algorithm/string/predicate.hpp>
#include <utility>
#include <obelisk/http/framework/config.h>
#include "col.h"
#include <obelisk/database/core/common.h>

namespace obelisk::database::builder::detail
{
    using condition_operand_t = std::variant<col, core::sql_value>;

    class condition_operands final : public core::base_statement
    {
    public:
        template<typename T>
        condition_operands(const T& value): var_(core::sql_value(value)){}
        std::string compile() override
        {
            return std::visit(core::general_visitor{}, var_);
        }
        condition_operands(col value): var_(std::move(value)){}

        [[nodiscard]] bool is_null() const
        {
            return var_.index() == 1 && std::get<core::sql_value>(var_).index() == 0;
        }

    private:
        condition_operand_t var_;
    };

    class condition : public core::base_statement {
    public:
        condition(condition_operands first, condition_operands  second): first_(std::move(first)), second_(std::move(second)){}
        condition(condition_operands first, std::string operator_string, condition_operands  second): first_(std::move(first)), second_(std::move(second)), operator_(std::move(operator_string)){}
        std::string compile() override
        {
            return std::format("{} {} {}", first_.compile(), second_.is_null()? "IS":operator_, second_.compile());
        };
    private:
        condition_operands first_, second_;
        std::string operator_ = "=";
    };
}
#endif //OBELISK_DATABASE_BUILDER_DETAIL_CONDITION_H
