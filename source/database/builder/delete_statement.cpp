
#include <obelisk/database/builder/delete_statement.h>
namespace obelisk::database::builder
{
    std::string delete_statement::compile()
    {
        std::string result = std::format("DELETE FROM {} ", utils::separate_with(tables_, ","));
        if (!where_groups_.empty())
        {
            result.append(std::format("WHERE {} ", utils::separate_with(where_groups_ ," OR ")));
        }
        result.append(";");
        return result;
    }

}