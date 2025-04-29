//
// Created by hackman on 4/29/25.
//

#ifndef VALIDATE_VERIFY_CODE_H
#define VALIDATE_VERIFY_CODE_H
#include <boost/asio/awaitable.hpp>

namespace default_::utils
{
    inline boost::asio::awaitable<bool> validate_verify_code(const std::string& phone, const std::string& verify_code, const std::string& type)
    {
        co_return true;
    }
}

#endif //VALIDATE_VERIFY_CODE_H
