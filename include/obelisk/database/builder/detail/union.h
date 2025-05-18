//
// Created by hackman on 5/18/25.
//

#ifndef OBELISK_DATABASE_BUILDER_UNION_H
#define OBELISK_DATABASE_BUILDER_UNION_H
#include <vector>
#include <obelisk/database/core/common.h>

#include "sub_query.h"

namespace obelisk::database::builder::detail
{
    using union_t = std::variant<std::vector<core::sql_value>, sub_query>;

    class union_ : public core::base_statement
    {
    public:
        template<typename T>
        requires (std::is_base_of_v<base_statement, T>)
        union_(const T& tar): target_(tar){ }
        union_(const std::initializer_list<core::sql_value>& tar)
        {
            std::vector<core::sql_value> result;
            std::ranges::copy(tar, std::back_inserter(result));
            target_ = std::move(result);
        }

        std::string compile() override;

    protected:
        union_t target_;
    };
}

#endif //OBELISK_DATABASE_BUILDER_UNION_H
