//
// Created by hackman on 5/19/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_GROUP_BY_H
#define OBELISK_DATABASE_BUILDER_DETAIL_GROUP_BY_H
#include "col.h"
#include "common.h"
#include "utils.h"

namespace obelisk::database::builder::detail
{
    class group_by_t : public base_statement{
    public:
        group_by_t(const col& col):source_({col}){}
        group_by_t(const std::initializer_list<col>& col):source_(col){}

        std::string compile() override
        {
            std::string result = std::format("GROUP BY {} ", utils::separate_with(source_, ","));
            return result;
        }
    private:
        std::vector<col> source_;
    };

}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_GROUP_BY_H
