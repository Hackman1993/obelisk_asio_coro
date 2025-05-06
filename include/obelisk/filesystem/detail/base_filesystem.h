//
// Created by hackman on 5/6/25.
//

#ifndef BASE_FILESYSTEM_H
#define BASE_FILESYSTEM_H
#include <string>

namespace obelisk::fs::detail
{
    class base_filesystem {
    public:
        virtual bool exists(const std::string& path) = 0;
        virtual bool save(const std::string& path, const std::istream& file) = 0;
        virtual bool remove(const std::string& path) = 0;
    };

}

#endif //BASE_FILESYSTEM_H
