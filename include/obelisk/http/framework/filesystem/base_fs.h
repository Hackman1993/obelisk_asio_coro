//
// Created by hackman on 5/9/25.
//

#ifndef BASE_FS_H
#define BASE_FS_H
#include <string>

namespace obelisk::http
{
    class base_fs {
    public:
        virtual ~base_fs() = default;
        virtual boost::asio::awaitable<bool> exists(const std::string& path) = 0;
        virtual boost::asio::awaitable<bool> save(const std::string& path, std::unique_ptr<std::iostream> file) = 0;
        virtual boost::asio::awaitable<bool> remove(const std::string& path) = 0;
    };

}

#endif //BASE_FS_H
