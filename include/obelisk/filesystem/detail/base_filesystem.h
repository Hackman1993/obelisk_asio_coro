//
// Created by hackman on 5/6/25.
//

#ifndef BASE_FILESYSTEM_H
#define BASE_FILESYSTEM_H
#include <string>
#include <utility>

namespace obelisk::fs::detail
{
    class base_filesystem {
    public:
        virtual ~base_filesystem() = default;
        virtual boost::asio::awaitable<bool> exists(const std::string& path) = 0;
        virtual boost::asio::awaitable<std::string> save(const std::string& path, std::unique_ptr<std::iostream> file) = 0;
        virtual boost::asio::awaitable<std::string> save_random_name(const std::string& path, http::http_file& file) = 0;
        virtual boost::asio::awaitable<bool> remove(const std::string& path) = 0;
    };
}

#endif //BASE_FILESYSTEM_H
