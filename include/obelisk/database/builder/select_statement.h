//
// Created by hackman on 5/17/25.
//

#ifndef SELECT_STATEMENT_H
#define SELECT_STATEMENT_H
#include "detail/query.h"

namespace obelisk::database::builder
{

    class select_statement : public detail::query{
    public:
        std::string compile() override;
    };

}

#endif //SELECT_STATEMENT_H
