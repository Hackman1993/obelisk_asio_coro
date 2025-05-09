//
// Created by hackman on 5/9/25.
//

#ifndef BASE_FS_H
#define BASE_FS_H
#include <string>

namespace obelisk::http
{
    using namespace boost::asio;
    class base_fs {
    public:
        virtual ~base_fs() = default;
        virtual awaitable<bool> exists(const std::string& path) = 0;
        virtual awaitable<bool> save(const std::string& path, const std::istream& file) = 0;
        virtual awaitable<bool> remove(const std::string& path) = 0;
    };

}

#endif //BASE_FS_H
