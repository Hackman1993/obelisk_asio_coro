//
// Created by hackman on 5/18/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_SUB_QUERY_H
#define OBELISK_DATABASE_BUILDER_DETAIL_SUB_QUERY_H
#include <functional>
#include "common.h"
namespace obelisk::database::builder::detail
{
    class query;


    class sub_query : public base_statement
    {
    public:
        sub_query(const std::function<void(query&)>& builder): builder_(builder){}
        std::string compile() override;

    private:
        std::optional<std::function<void(query&)>> builder_;
    };

}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_RAW_H
