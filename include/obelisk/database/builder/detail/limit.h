//
// Created by hackman on 5/19/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_LIMIT_H
#define OBELISK_DATABASE_BUILDER_DETAIL_LIMIT_H
#include <obelisk/database/builder/base_statement.h>
#include <obelisk/database/core/common.h>
namespace obelisk::database::builder::detail
{

    class limit_t final : public base_statement{
    public:
        limit_t(const std::uint64_t limit, const std::uint64_t offset = 0) : limit_(limit), offset_(offset) {}
        std::string compile() override
        {
            return std::format("LIMIT {} OFFSET {}", limit_, offset_);
        }

    private:
        std::uint64_t limit_;
        std::uint64_t offset_ = 0;
    };

}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_LIMIT_H
