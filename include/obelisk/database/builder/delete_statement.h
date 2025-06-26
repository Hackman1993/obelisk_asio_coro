//
// Created by hackman on 5/17/25.
//

#ifndef DELETE_STATEMENT_H
#define DELETE_STATEMENT_H
#include "detail/common.h"
#include "detail/enable_where_condition.h"
#include "detail/table.h"

namespace obelisk::database::builder
{

    class delete_statement : public detail::enable_where_condition<delete_statement>, public detail::base_builder_statement
    {
    public:
        std::string compile() override;
        explicit delete_statement(const std::vector<detail::table>& tables){ tables_ = tables;}

    private:
        std::vector<detail::table> tables_;
    };

}

#endif //DELETE_STATEMENT_H
