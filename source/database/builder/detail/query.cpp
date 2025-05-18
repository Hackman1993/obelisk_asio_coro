#include "obelisk/database/builder/detail/query.h"
//
// Created by hackman on 5/18/25.
//
namespace obelisk::database::builder::detail
{
    std::string query::compile()
    {
        std::string result = std::format("SELECT {}\n\t",distinct_? "DISTINCT ": "");
        result.append(std::format("{} ", utils::separate_with(selects_, ",\n\t")));
        if (!from_.empty())
            result.append(std::format("FROM {}\n", utils::separate_with(from_, ",")));
        if (!joins_.empty())
            result.append(std::format("\t {} ", utils::separate_with(joins_, "\n\t")));
        if (!this->where_groups_.empty())
            result.append(std::format("WHERE {} ", utils::separate_with(this->where_groups_ ," OR ")));
        if (order_by_)
            result.append(std::format("ORDER BY {} ", order_by_.value().compile()));
        if (!unions_.empty())
            result.append(std::format("UNION {} ", utils::separate_with(unions_, "\nUNION ")));
        result.append(";");
        return result;
    }

    query& query::join(join_ join){
        joins_.emplace_back(std::move(join));
        return *this;
    }

    query& query::join(join_t join_table, std::vector<condition> conditions){
        joins_.emplace_back(std::move(join_table), std::move(conditions));
        return *this;
    }

}