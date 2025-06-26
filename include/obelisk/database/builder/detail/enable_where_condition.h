//
// Created by hackman on 5/25/25.
//

#ifndef OBELISK_DATABASE_BUILDER_ENABLE_WHERE_CONDITION_H
#define OBELISK_DATABASE_BUILDER_ENABLE_WHERE_CONDITION_H
#include "condition_group.h"
namespace obelisk::database::builder::detail
{

    template <typename T, bool AppendWhereKeyword = true>
    class enable_where_condition
    {
    public:
        T& where(const std::initializer_list<condition>& conditions) {
            if (where_groups_.empty())
                where_groups_.emplace_back();

            auto &where = where_groups_.back();
            std::ranges::copy(conditions,std::back_inserter( where));
            return static_cast<T&>(*this);
        }

        T& or_where(const std::initializer_list<condition>& conditions)
        {
            where_groups_.emplace_back();
            auto &where = where_groups_.back();
            std::ranges::copy(conditions,std::back_inserter( where));
            return static_cast<T&>(*this);
        }

        T& global_where(const std::initializer_list<condition>& conditions)
        {
            std::ranges::copy(conditions,std::back_inserter( global_where_));
            return static_cast<T&>(*this);
        }

        std::string compile_where()
        {
            std::string result;
            if ((!where_groups_.empty() || !global_where_.empty())&& AppendWhereKeyword)
                result.append("WHERE ");
            if (!where_groups_.empty()){
                if (global_where_.empty() || where_groups_.size() == 1)
                    result.append(std::format("{} ", utils::separate_with(where_groups_, " OR ")));
                else
                    result.append(std::format("({}) ", utils::separate_with(where_groups_, " OR ")));
            }
            if (!global_where_.empty())
            {
                if (!where_groups_.empty())
                    result.append("AND ");
                result.append(std::format("({}) ", utils::separate_with(global_where_, " AND ")));
            }
            return result;
        }


    protected:
        std::vector<condition_group> where_groups_;
        std::vector<condition> global_where_;
    };

}

#endif //OBELISK_DATABASE_BUILDER_ENABLE_WHERE_CONDITION_H
