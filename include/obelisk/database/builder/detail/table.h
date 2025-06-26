//
// Created by hackman on 4/27/25.
//

#ifndef OBELISK_DATABASE_BUILDER_TABLE_H
#define OBELISK_DATABASE_BUILDER_TABLE_H

#include "common.h"

namespace obelisk::database::builder::detail
{
    class table final : public base_statement
    {
    public:
        table() = default;
        table(const table& tb){ table_ = tb.table_; }
        table(std::string str): table_(std::move(str)){ }
        table(const char* str): table(std::string(str)){}

        std::string compile() override;

    private:
        std::string table_;
    };
}
#endif //OBELISK_DATABASE_BUILDER_TABLE_H
