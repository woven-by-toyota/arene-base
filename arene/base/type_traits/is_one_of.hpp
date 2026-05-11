// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ONE_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ONE_OF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/any_of.hpp"

namespace arene {
namespace base {

/// @brief Query if the type 'T' is one of the list of types 'Ts...'
///
/// Evaluates to 'true' if 'T' is a member of the set 'Ts...',
/// otherwise to 'false'.
///
/// @tparam T The type to search for in TL.
/// @tparam TL The TypeList to search in for T.
template <typename T, typename... Ts>
constexpr bool is_one_of_v = any_of_v<std::is_same<T, Ts>::value...>;

/// @brief Query if the type 'T' is one of the list of types 'Ts...'
///
/// Inherits from std::true_type if 'T' is a member of the set 'Ts...',
/// otherwise inherits from std::false_type.
///
/// @tparam T The type to search for in TL.
/// @tparam TL The TypeList to search in for T.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T, typename... Ts>
class is_one_of : public std::integral_constant<bool, is_one_of_v<T, Ts...>> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ONE_OF_HPP_
