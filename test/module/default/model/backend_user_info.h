//
// Created by hackman on 5/1/25.
//

#ifndef BACKEND_USER_INFO_H
#define BACKEND_USER_INFO_H
#include <string>
#include <cstdint>
namespace module::default_::model
{

    struct backend_user_info {
        std::uint64_t id;
        std::uint64_t fn_organization_id;
        std::string username;
    };

}

#endif //BACKEND_USER_INFO_H
