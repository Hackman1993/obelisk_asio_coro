//
// Created by hackman on 4/27/25.
//

#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>

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

#endif //UTILS_H
