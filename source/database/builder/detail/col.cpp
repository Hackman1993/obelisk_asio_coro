#include <obelisk/database/builder/detail/col.h>
#include <obelisk/http/framework/config.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
namespace obelisk::database::builder::detail
{
    std::string col::compile()
    {
        auto prefix = http::config::get<std::string>("database.default.prefix", "");
        const bool contains_table_name = boost::algorithm::contains(col_, ".");
        auto column_name = col_;
        if (contains_table_name)
            boost::algorithm::replace_all(column_name, ".", "`.`");
        std::string result = std::format("`{}{}`", contains_table_name? prefix:"", column_name);
        return result;
    }
}