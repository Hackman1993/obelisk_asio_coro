#include <obelisk/database/builder/select_statement.h>
namespace obelisk::database::builder
{
    std::string select_statement::compile()
    {
        return query::compile() + ";";
    }
}
