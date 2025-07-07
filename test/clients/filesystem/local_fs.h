#ifndef LOCAL_FS_H
#define LOCAL_FS_H
#include <filesystem>
#include <format>
#include <fstream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/asio/awaitable.hpp>
#include "obelisk/filesystem/detail/base_filesystem.h"
class local_fs final : public obelisk::fs::detail::base_filesystem{
public:

    local_fs(const std::string& local_path, const std::string& access_url):local_path_(local_path), access_url_(access_url)
    {
    }

    boost::asio::awaitable<bool> exists(const std::string& path) override
    {
        std::string local_path = std::format("{}{}", local_path_, secure_path_(path));
        co_return std::filesystem::exists(local_path);
    }
    boost::asio::awaitable<std::string> save(const std::string& path, std::unique_ptr<std::iostream> file_data) override
    {
        auto secure_path =  secure_path_(path);
        std::string local_path = std::format("{}{}", local_path_,secure_path);
        std::fstream file(local_path, std::ios::binary | std::ios::out);
        std::copy(std::istream_iterator<char>(*file_data), std::istream_iterator<char>(), std::ostreambuf_iterator(file));
        co_return std::format("{}{}",access_url_, secure_path);
    }

    boost::asio::awaitable<std::string> save_random_name(const std::string& path, obelisk::http::http_file& file) override
    {
        const std::string& filename = std::format("{}{}", sahara::utils::uuid::generate(), file.extension_.has_value()?file.extension_.value(): "");
        std::string save_path = path;
        if (!path.ends_with("/"))
            save_path += "/";
        save_path += filename;
        auto secure_path = secure_path_(save_path);
        const std::string local_path = std::format("{}{}", local_path_,secure_path);
        const std::filesystem::path fs_path(local_path);
        if (!std::filesystem::exists(fs_path.parent_path()))
            std::filesystem::create_directories(fs_path.parent_path());
        std::filesystem::copy_file(file.temp_path_, fs_path);
        co_return std::format("{}{}",access_url_, secure_path);
    }

    boost::asio::awaitable<bool> remove(const std::string& path) override
    {
        if (std::string local_path = std::format("{}{}", local_path_, secure_path_(path)); std::filesystem::exists(local_path))
            std::filesystem::remove(local_path);
        co_return true;
    }
private:
    std::string local_path_;
    std::string access_url_;

    static std::string secure_path_(const std::string& path)
    {
        auto result = path;
        boost::algorithm::replace_all(result, "\\", "/");

        while (result.contains(".."))
            boost::algorithm::replace_all(result, "..", "");
        while (result.contains("./"))
            boost::algorithm::replace_all(result, "./", "/");
        while (result.contains("//"))
            boost::algorithm::replace_all(result, "//", "/");
        if (result.starts_with("/"))
            return result;
        return std::format("/{}", result);
    }
};



#endif //LOCAL_FS_H
