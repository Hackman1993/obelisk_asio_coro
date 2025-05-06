//
// Created by hackman on 5/6/25.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <boost/asio/awaitable.hpp>
#include <obelisk/http/framework/config.h>
#include <sahara/exception/exception_base.h>

#include "detail/base_filesystem.h"

namespace obelisk {

class filesystem {
    public:

        static std::weak_ptr<fs::detail::base_filesystem> guard(const std::string& key)
        {
            if (!self().connections_.contains(key))
                THROW(sahara::exception::exception_base,"Invalid Filesystem Guard", "FileSystem");
            return self().connections_[key];
        }
        filesystem(const filesystem&) = delete;
        filesystem& operator=(const filesystem&) = delete;
    protected:
        filesystem() = default;
        ~filesystem() = default;
        static filesystem& self()
        {
            http::config::get<()
            static filesystem inst;
            return inst;
        }
        std::unordered_map<std::string, std::shared_ptr<fs::detail::base_filesystem>> connections_;
    };
} // obelisk

#endif //FILESYSTEM_H
