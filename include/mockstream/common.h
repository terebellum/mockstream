#pragma once

#include <type_traits>

namespace mockstream {

/*
   is_unique - checks that parameter pack contains only unique values, O(n^2)
*/
template <typename...>
inline constexpr auto is_unique = std::true_type{};

template <typename T, typename... Rest>
inline constexpr auto is_unique<T, Rest...> =
    std::bool_constant<(!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>>{};

}  // namespace mockstream
