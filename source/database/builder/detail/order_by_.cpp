#include <obelisk/database/builder/detail/order_by_.h>
#include <obelisk/database/builder/detail/utils.h>

namespace obelisk::database::builder::detail
{
    std::string order_by_::compile()
    {
        std::string result = std::format("{} ", utils::separate_with(columns_, ","));
        if (order_)
            result.append(std::format("{} ", order_.value()));
        return result;
    }
}
