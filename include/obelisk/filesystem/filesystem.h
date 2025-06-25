//
// Created by hackman on 5/6/25.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <boost/asio/awaitable.hpp>
#include <format>
#include <obelisk/http/framework/config.h>
#include <sahara/exception/exception_base.h>

#include "detail/base_filesystem.h"

namespace obelisk {

class filesystem {
    public:

        static std::shared_ptr<fs::detail::base_filesystem> guard(const std::string& key)
        {
            if (!self().connections_.contains(key))
                THROW(sahara::exception::exception_base,"Invalid Filesystem Guard", "FileSystem");
            return self().connections_[key];
        }
        filesystem(const filesystem&) = delete;
        filesystem& operator=(const filesystem&) = delete;
        static void initialize()
        {
            for (const auto fs = http::config::get<nlohmann::json::object_t>("storage", nlohmann::json::object_t{}); const auto &config: fs)
            {
                const auto &key = config.second["type"].get<std::string>();
                if (!self().maker_.contains(key))
                    THROW(sahara::exception::exception_base, std::format("fs type {} not registered!", key), "FileSystem");
                auto ptr = self().maker_[key](config.second);
                if (!ptr)
                    THROW(sahara::exception::exception_base, "Filesystem register returns null!", "FileSystem");
                self().connections_.emplace(config.first, ptr);
            }
        }
        static void register_fs(const std::string& key, const std::function<std::shared_ptr<fs::detail::base_filesystem>(const nlohmann::json& config)>& maker)
        {
            self().maker_.emplace(key, maker);
        }
    protected:
        filesystem() = default;
        ~filesystem() = default;
        static filesystem& self()
        {
            static filesystem inst;
            return inst;
        }
        std::unordered_map<std::string, std::function<std::shared_ptr<fs::detail::base_filesystem>(const nlohmann::json& config)>> maker_ = {};
        std::unordered_map<std::string, std::shared_ptr<fs::detail::base_filesystem>> connections_;
    };
} // obelisk

#endif //FILESYSTEM_H
