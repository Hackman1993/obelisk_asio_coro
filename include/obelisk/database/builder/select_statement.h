//
// Created by hackman on 5/17/25.
//

#ifndef SELECT_STATEMENT_H
#define SELECT_STATEMENT_H
#include "detail/enable_execute.h"
#include "detail/query.h"

namespace obelisk::database::builder
{

    class select_statement : public detail::query, public detail::enable_execute{

    };

}

#endif //SELECT_STATEMENT_H
