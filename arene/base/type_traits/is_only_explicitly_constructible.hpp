// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ONLY_EXPLICITLY_CONSTRUCTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ONLY_EXPLICITLY_CONSTRUCTIBLE_HPP_

// IWYU pragma: begin_keep
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
// IWYU pragma: end_keep

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Everything is preceeded by a comment with @brief"

namespace arene {
namespace base {

/// @brief Type trait to detect if a type is explicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be explicitly constructed.
/// @tparam Args The arguments to try to explicitly construct with.
template <class T, class... Args>
extern constexpr bool is_only_explicitly_constructible_v =
    std::is_constructible<T, Args...>::value && !is_implicitly_constructible_v<T, Args...>;

/// @brief Type trait to detect if a type is explicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be explicitly constructed.
/// @tparam Args The arguments to try to explicitly construct with.
template <class T, class... Args>
class is_only_explicitly_constructible
    : public std::integral_constant<bool, is_only_explicitly_constructible_v<T, Args...> > {};

/// @brief Type trait to detect if a type is nothrow explicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be nothrow explicitly constructed.
/// @tparam Args The arguments to try to nothrow explicitly construct with.
template <class T, class... Args>
extern constexpr bool is_nothrow_only_explicitly_constructible_v =
    std::is_nothrow_constructible<T, Args...>::value && !is_nothrow_implicitly_constructible_v<T, Args...>;

/// @brief Type trait to detect if a type is nothrow explicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be nothrow explicitly constructed.
/// @tparam Args The arguments to try to nothrow explicitly construct with.
template <class T, class... Args>
class is_nothrow_only_explicitly_constructible
    : public std::integral_constant<bool, is_nothrow_only_explicitly_constructible_v<T, Args...> > {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ONLY_EXPLICITLY_CONSTRUCTIBLE_HPP_
