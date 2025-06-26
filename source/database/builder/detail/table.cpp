#include <obelisk/database/builder/detail/table.h>
#include <obelisk/http/framework/config.h>
namespace obelisk::database::builder::detail
{
    std::string table::compile()
    {
        auto prefix = http::config::get<std::string>("database.default.prefix", "");
        return std::format("`{}{}`", prefix, table_);
    }
}