//
// Created by hackman on 5/19/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_COUNT_H
#define OBELISK_DATABASE_BUILDER_DETAIL_COUNT_H
#include "col.h"
#include <obelisk/database/core/common.h>

#include "raw.h"

namespace obelisk::database::builder::detail
{

    using countable_t = std::variant<col, raw, core::sql_value>;

    class count_t : public core::base_statement{
    public:
        template<typename T>
        requires std::is_convertible_v<T, countable_t>
        count_t(const T& source) : source_(source) {}

        count_t& distinct()
        {
            distinct_ = true;
            return *this;
        }
        std::string compile() override
        {
            std::string result = std::format("COUNT({}{})",distinct_? "DISTINCT ":"" , std::visit(core::general_visitor{}, source_));
            return result;
        }
    private:
        bool distinct_ = false;
        countable_t source_;
        std::optional<std::string> separator_;
    };

}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_COUNT_H
