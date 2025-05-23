#include <obelisk/database/builder/insert_statement.h>

namespace obelisk::database::builder
{
    insert_statement& insert_statement::into(const detail::table& tb)
    {
        table_ = tb;
        return *this;
    }
    std::string insert_statement::compile()
    {
        std::string result = std::format("INSERT INTO {}({}) ", table_.compile(), utils::separate_with(insert_columns_, ","));
        if (!insert_values_.empty())
        {
            result.append(std::format("VALUES {} ", utils::separate_with(insert_values_, ",")));
        }else if (sub_query_)
        {
            result.append(sub_query_->compile());
        }


        result.append(";");
        return result;
    }
}