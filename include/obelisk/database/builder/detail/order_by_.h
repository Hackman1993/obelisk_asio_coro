//
// Created by hackman on 5/17/25.
//

#ifndef ORDER_BY_H
#define ORDER_BY_H
#include "col.h"
#include <obelisk/database/core/common.h>
namespace obelisk::database::builder::detail
{

    class order_by_ final : core::base_statement{
    public:
        order_by_(std::vector<col> columns):columns_(std::move(columns)){}
        order_by_(std::vector<col> columns, std::string order):columns_(std::move(columns)), order_(std::move(order)){}

        std::string compile() override;

    private:
        std::vector<col> columns_;
        std::optional<std::string> order_;
    };

}

#endif //ORDER_BY_H
