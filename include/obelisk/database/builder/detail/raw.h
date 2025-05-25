//
// Created by hackman on 4/27/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_RAW_H
#define OBELISK_DATABASE_BUILDER_DETAIL_RAW_H
#include <utility>
#include "common.h"

namespace obelisk::database::builder::detail
{
    class raw final : public base_statement
    {
    public:
        raw() = default;
        explicit raw(std::string sql) : sql_(std::move(sql)) {}
        std::string compile() override{ return sql_;}

    private:
        std::string sql_;
    };
}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_RAW_H
