#include <obelisk/database/builder/base_statement.h>
#include <obelisk/database/builder/detail/union.h>
#include <obelisk/database/builder/detail/utils.h>

namespace obelisk::database::builder::detail
{
    struct union_visitor
    {
        template<typename T>
        requires std::is_base_of_v<core::base_statement, T>
        std::string operator()(T& c)
        {
            return c.compile();
        }

        std::string operator()(std::vector<core::sql_value>& c) const
        {
            return std::format("SELECT {}", utils::separate_with(c, ","));
        }
    };
    std::string union_::compile()
    {
        return std::format("({})", std::visit(union_visitor{}, target_));
    }

}
