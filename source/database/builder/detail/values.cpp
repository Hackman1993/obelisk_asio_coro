//
// Created by hackman on 5/18/25.
//

#include <obelisk/database/builder/detail/utils.h>
#include <obelisk/database/builder/detail/values.h>

namespace obelisk::database::builder::detail
{
    values_t::values_t(std::initializer_list<core::sql_value> values)
    {
        std::ranges::copy(values, std::back_inserter(values_));
    }

    std::string values_t::compile()
    {
        return std::format("({})", utils::separate_with(values_, ","));
    }

}