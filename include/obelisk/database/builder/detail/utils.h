//
// Created by hackman on 4/27/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_UTILS_H
#define OBELISK_DATABASE_BUILDER_DETAIL_UTILS_H
#include <string>
#include <vector>
namespace obelisk::database::builder::detail
{
    class utils
    {
    public:
        template <typename T>
        static std::string separate_with(std::vector<T>& vec, const std::string& separator)
        {
            std::string result;
            for (int i = 0; i < vec.size(); ++i)
            {
                if (i > 0)
                    result.append(separator);
                result.append(vec[i].compile());
            }
            return result;
        }

    private:
        utils() = default;
    };
}
class utils
{
public:
    template <typename T>
    static std::string separate_with(std::vector<T>& vec, const std::string& separator)
    {
        std::string result;
        for (int i = 0; i < vec.size(); ++i)
        {
            if (i > 0)
                result.append(separator);
            result.append(vec[i].compile());
        }
        return result;
    }

private:
    utils() = default;
};
#endif //OBELISK_DATABASE_BUILDER_DETAIL_UTILS_H
