// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_SIZE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_SIZE_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_list/type_list.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A14_7_2-a-2 "False positive: 'type_list' is defined in type_list.hpp included above"
/// @brief Specialize @c tuple_size for @c type_list
template <class... Tn>
class std::tuple_size<arene::base::type_list<Tn...>> : public std::integral_constant<std::size_t, sizeof...(Tn)> {};
// parasoft-end-suppress AUTOSAR-A14_7_2-a-2

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace size_detail {

/// @brief Check for specialization of @c std::tuple_size<>.
/// @tparam T The type to check.
template <class T, class = constraints<>>
class has_tuple_size_impl : public std::false_type {};

/// @brief Check for specialization of @c std::tuple_size<>.
/// @tparam T The type to check.
template <class T>
class has_tuple_size_impl<T, constraints<decltype(std::tuple_size<T>::value)>> : public std::true_type {};

/// @brief Get the count of elements in a type-list.
/// @tparam T The type-list for which to get the count of elements.
template <class T, class = constraints<>>
class size_impl;

/// @brief Get the count of elements in a type-list.
/// @tparam L0 The type-list template
/// @tparam Tn The types in the list
template <template <class...> class L0, class... Tn>
class size_impl<
    L0<Tn...>,
    constraints<  //
        std::enable_if_t<!has_tuple_size_impl<L0<Tn...>>::value>>>
    : public std::integral_constant<std::size_t, sizeof...(Tn)> {};

/// @brief Get the count of elements in a type-list that has a @c tuple_size specialization.
/// @tparam T The type-list for which to get the count of elements.
template <class T>
class size_impl<
    T,
    constraints<  //
        std::enable_if_t<has_tuple_size_impl<T>::value>>> : public std::tuple_size<T> {};

}  // namespace size_detail

/// @endcond

/// @brief Get the size of the type list.
///
/// @tparam L0 An instantiation of a type-list that holds the types to count
/// @pre @c L0 is a type-list
/// @return The size of the type list.
template <class L0>
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
constexpr auto size() -> std::size_t {
  return arene::base::type_lists::size_detail::size_impl<L0>::value;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Get the size of the type list.
///
/// @tparam L0 An instantiation of a type-list that holds the types to count
/// @pre @c L0 is a type-list
/// @return The size of the type list.
template <class L0>
ARENE_MAYBE_UNUSED extern constexpr std::size_t size_v{size<L0>()};

// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: size_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: size_t not reserved in this context"
/// @brief Get the size of the type list.
///
/// @tparam L0 An instantiation of a type-list that holds the types to count
/// @pre @c L0 is a type-list
/// @return The size of the type list.
template <class L0>
using size_t = arene::base::type_lists::size_detail::size_impl<L0>;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3

/// @brief Get whether the type list is empty.
///
/// @tparam L0 An instantiation of a type-list that holds the types to count
/// @pre @c L0 is a type-list
/// @return True if the type list is empty.
template <class L0>
constexpr auto empty() -> bool {
  return arene::base::type_lists::size_v<L0> == 0UL;
}

/// @brief Get whether the type list is empty.
///
/// @tparam L0 An instantiation of a type-list that holds the types to count
/// @pre @c L0 is a type-list
/// @return True if the type list is empty.
template <class L0>
ARENE_MAYBE_UNUSED extern constexpr bool empty_v{empty<L0>()};

/// @brief Get whether the type list is empty.
///
/// @tparam L0 An instantiation of a type-list that holds the types to count
/// @pre @c L0 is a type-list
/// @return True if the type list is empty.
template <class L0>
struct empty_t {
  // parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a-2 "False positive: initializer is constant"
  /// @brief The Result of the check
  static constexpr bool value{empty_v<L0>};
  // parasoft-end-suppress AUTOSAR-A3_3_2-a-2
  // parasoft-end-suppress CERT_CPP-DCL56-a-3
};

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_SIZE_HPP_
