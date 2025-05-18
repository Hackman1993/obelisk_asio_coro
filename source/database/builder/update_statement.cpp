//
// Created by hackman on 5/17/25.
//
#include <obelisk/database/builder/update_statement.h>
namespace obelisk::database::builder
{
    std::string update_statement::compile()
    {
        std::string result;
        std::string set_values;
        for (int i =0; i< set_.size(); ++i)
        {
            if (i != 0) set_values.append(",");
            set_values.append(std::format("{} = {}", set_[i].first.compile(), set_[i].second.compile()));
        }
        result.append(std::format("UPDATE {} SET {} ", utils::separate_with(tables_, ","), set_values));
        if (!where_groups_.empty())
        {
            result.append(std::format("WHERE {} ", utils::separate_with(where_groups_ ," OR ")));
        }
        return result;
    }

    update_statement& update_statement::set(const std::vector<std::pair<detail::col, core::sql_value>>& values)
    {
        std::ranges::copy(values, std::back_inserter(set_));
        return *this;
    }

}