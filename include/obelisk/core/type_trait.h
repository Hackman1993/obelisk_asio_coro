//
// Created by hackman on 5/17/25.
//

#ifndef TYPE_TRAIT_H
#define TYPE_TRAIT_H
#include <type_traits>
#include <variant>
namespace obelisk::core
{

    template <typename T>
    struct is_variant : std::false_type {};

    template <typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};

    template <typename T>
    inline constexpr bool is_variant_v = is_variant<T>::value;

}

#endif //TYPE_TRAIT_H
