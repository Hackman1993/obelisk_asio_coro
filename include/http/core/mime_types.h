/***********************************************************************************************************************
 * @author Hackman Lo
 * @file mime_types.h
 * @description 
 * @created_at 2023-10-10
***********************************************************************************************************************/

#ifndef OBELISK_MIME_TYPES_H
#define OBELISK_MIME_TYPES_H
#include <string>
#include <unordered_map>
namespace obelisk {

    class mime_types {
    public:
        static std::string lookup(const std::string& extension);
    protected:
        mime_types() = default;
        static std::unordered_map<std::string, std::string> mimes_;
    };

} // obelisk

#endif //OBELISK_MIME_TYPES_H
