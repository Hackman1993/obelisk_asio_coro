//
// Created by hackman on 4/25/25.
//

#ifndef OBELISK_FRAMEWORK_CONFIG_H
#define OBELISK_FRAMEWORK_CONFIG_H
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/detail/classification.hpp>
#include <boost/asio/awaitable.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace obelisk::http
{
    class config
    {
    public:
        config(const config&) = delete;
        config& operator=(const config&) = delete;


        template <typename T>
        static T get(const std::string& path, T default_val)
        {
            std::vector<std::string_view> keys;
            split(keys, path, boost::algorithm::is_any_of("."));
            std::string current_path;
            current_path.reserve(path.length());

            nlohmann::json* current = &self().config_;
            for (int i = 0; i < keys.size(); ++i)
            {
                current_path.append(i == 0 ? "" : ".");
                current_path.append(keys[i]);

                if (i == keys.size() - 1)
                {
                    return current->contains(keys[i]) ? (*current)[keys[i]].get<T>() : default_val;
                }
                if (!current->contains(keys[i]))
                    return default_val;
                current = &(*current)[keys[i]];
            }
            return default_val;
        }

    private:
        static config& self()
        {
            static config instance;
            return instance;
        }

        config()
        {
            nlohmann::json config;
            std::ifstream fs("./config.json");
            if (fs.fail())
            {
                std::cout << "Error details: " << strerror(errno)
                    << std::endl;
            }
            config_ = nlohmann::json::parse(fs);
        };
        ~config() = default;

        nlohmann::json config_;
    };
}

#endif //OBELISK_FRAMEWORK_CONFIG_H
