// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::contains"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONTAINS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONTAINS_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_traits/is_one_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace contains_detail {

/// @brief true if T is same as one of U... in L0 (aka TypeList0<U...>)
template <class L0, class T>
class contains_impl;

/// @brief Implementation class for @c contains
/// @tparam TypeList0 The type list template
/// @tparam T0n The elements of the type list
/// @tparam T The element to search for
template <template <class...> class TypeList0, class... T0n, class T>
class contains_impl<TypeList0<T0n...>, T> : public std::integral_constant<bool, is_one_of<T, T0n...>::value> {};

}  // namespace contains_detail

/// @endcond

/// @brief Query if a type-list contains a given type.
///
/// Evaluates to 'true' if T is an element of the type-list, L0,
/// otherwise evaluates to 'false'.
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for.
/// @pre @c L0 is a type-list
/// @return bool
template <class L0, class T>
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
constexpr auto contains() -> bool {
  return arene::base::type_lists::contains_detail::contains_impl<L0, T>::value;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Query if a type-list contains a given type.
///
/// Evaluates to 'true' if T is an element of the type-list, L0,
/// otherwise evaluates to 'false'.
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for.
/// @pre @c L0 is a type-list
/// @return bool
template <class L0, class T>
ARENE_MAYBE_UNUSED static constexpr bool contains_v{contains<L0, T>()};

/// @brief Query if a type-list contains a given type.
///
/// Evaluates to 'true' if T is an element of the type-list, L0,
/// otherwise evaluates to 'false'.
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for.
/// @pre @c L0 is a type-list
template <class L0, class T>
struct contains_t {
  // parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a-2 "False positive: initializer is constant"
  /// @brief The result of the query
  static constexpr bool value{contains_v<L0, T>};
  // parasoft-end-suppress AUTOSAR-A3_3_2-a-2
  // parasoft-end-suppress CERT_CPP-DCL56-a-3
};

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONTAINS_HPP_
