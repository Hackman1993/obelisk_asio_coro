//
// Created by hackman on 4/27/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_COL_H
#define OBELISK_DATABASE_BUILDER_DETAIL_COL_H
#include "common.h"
namespace obelisk::database::builder::detail
{
    class col final : public base_statement
    {
    public:
        col(const std::string& str): col_(str){ }
        col(const char* str): col_(str){ }
        std::string compile() override;
    private:
        std::string col_;
    };
}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_COL_H
