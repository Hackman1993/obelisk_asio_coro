#include <obelisk/database/builder/detail/query.h>
#include <obelisk/database/builder/detail/sub_query.h>

namespace obelisk::database::builder::detail
{
    std::string sub_query::compile()
    {
        query query;
        if (builder_)
        {
            builder_.value()(query.as_sub_query());
        }
        return query.compile();
    }
    
}
