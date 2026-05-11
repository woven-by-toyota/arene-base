// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::index_of"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_INDEX_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_INDEX_OF_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/type_traits/index_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace index_of_detail {

/// @brief Get the index of @c T in the type-list, @c L0.
///
/// Inherits from <c>std::integral_constant<std::size_t, Idx></c> where @c Idx
/// is the zero-based index of @c T in @c L0, if @c T is in @c L0.
///
/// If @c T is not in @c L0 then does not inherit from @c std::integral_constant
/// and the nested @c ::value member is not defined.
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for in @c L0.
template <class L0, class T>
class index_of_impl;

/// @brief Get the index of @c T in a type-list.
///
/// Inherits from <c>std::integral_constant<std::size_t, Idx></c> where @c Idx
/// is the zero-based index of @c T in the list, if @c T is in the list.
///
/// If @c T is not in the list then does not inherit from @c std::integral_constant
/// and the nested @c ::value member is not defined.
///
/// @tparam T The type to search for in the list.
/// @tparam TypeList0 The type list template
/// @tparam In The types in the list
template <class T, template <class...> class TypeList0, class... In>
class index_of_impl<TypeList0<In...>, T> : public index_of<T, In...> {};

}  // namespace index_of_detail

/// @endcond

/// @brief Get the index of @c T in the type-list, @c L0.
///
/// @pre @c T is in @c L0
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for in @c L0.
template <class L0, class T>
using index_of_t = arene::base::type_lists::index_of_detail::index_of_impl<L0, T>;

/// @brief Get the index of @c T in the type-list, @c L0.
///
/// @pre @c T is in @c L0
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for in @c L0.
template <class L0, class T>
ARENE_MAYBE_UNUSED extern constexpr std::size_t index_of_v = index_of_t<L0, T>::value;

/// @brief Get the index of @c T in the type-list, @c L0.
///
/// @pre @c T is in @c L0
///
/// @tparam L0 The type-list to search in.
/// @tparam T The type to search for in @c L0.
/// @return The index of @c T in @c L0
template <class L0, class T>
constexpr auto index_of() -> std::size_t {
  return arene::base::type_lists::index_of_v<L0, T>;
}

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_INDEX_OF_HPP_
