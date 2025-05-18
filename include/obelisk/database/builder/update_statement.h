//
// Created by hackman on 5/17/25.
//

#ifndef UPDATE_STATEMENT_H
#define UPDATE_STATEMENT_H
#include "detail/common.h"
#include "detail/enable_execute.h"
#include "detail/table.h"

namespace obelisk::database::builder
{

    class update_statement : public detail::enable_where_condition<update_statement>, public detail::base_builder_statement, public detail::enable_execute
    {
    public:
        update_statement(const std::vector<detail::table>& tables):tables_(tables){}
        std::string compile() override;
        update_statement& set(const std::vector<std::pair<detail::col, core::sql_value>>& values);
    private:
        std::vector<detail::table> tables_;
        std::vector<std::pair<detail::col, core::sql_value>> set_;
    };

}

#endif //UPDATE_STATEMENT_H
