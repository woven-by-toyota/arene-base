// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_INDEX_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_INDEX_OF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/conditional.hpp"

namespace arene {
namespace base {
namespace index_of_detail {

/// @brief Function to find the first true element in the list
/// @param values The list of elements to search
/// @return The index, or @c values.size() if not found
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
constexpr auto index_of_first_true(std::initializer_list<bool> values) noexcept -> std::size_t {
  std::size_t idx{0U};
  // parasoft-begin-suppress CERT_C-INT36-b-3 "False positive: const bool* not converted to integer"
  for (bool const value : values) {
    // parasoft-end-suppress CERT_C-INT36-b-3
    if (value) {
      return idx;
    }
    ++idx;
  }
  return idx;
}
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Function to find the last true element in the list
/// @param values The list of elements to search
/// @return The index, or @c values.size() if not found
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
constexpr auto index_of_last_true(std::initializer_list<bool> const values) noexcept -> std::size_t {
  // std::initializer_list has no operator[] but is contiguous so we can use its begin() pointer to index into it.
  bool const* const ptr{values.begin()};
  std::size_t idx{values.size()};
  while (idx != 0U) {
    --idx;
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "Either indexing or pointer arithmetic is required to iterate backward"
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (ptr[idx]) {
      return idx;
    }
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
  }
  return values.size();
}
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Template meta-function for querying the 0-based index of @c T within
/// a list of types, @c Ts... . It returns @c sizeof...(Ts) if the type is not
/// in the list.
///
/// @tparam T The type to search for.
/// @tparam Ts The list of types to search in.
template <typename T, typename... Ts>
constexpr std::size_t type_index_of_v = index_of_first_true({std::is_same<T, Ts>::value...});

/// @brief Template meta-function for querying the 0-based index of @c T within
/// a list of types, @c Ts... , starting from the end and searching backwards.
/// It returns @c sizeof...(Ts) if the type is not in the list.
///
/// @tparam T The type to search for.
/// @tparam Ts The list of types to search in.
template <typename T, typename... Ts>
constexpr std::size_t last_type_index_of_v = index_of_last_true({std::is_same<T, Ts>::value...});

/// @brief Base class with no @c type member for @c index_of
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
class empty_index_of_base {};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace index_of_detail

/// @brief Template meta-function for querying the 0-based index of a type in a list of types.
///
/// @tparam T The type to search for.
/// @tparam Ts The list of types to search in.
///
/// If @c T is found within @c Ts... then this type inherits from <c>std::integral_constant<std::size_t, Idx></c> where
/// @c Idx is the zero-based index of the first occurrence of @c T in @c Ts...
///
/// Access the result using the @c value member of this type. This type will not have a @c value member if @c T is not
/// one of the types listed in @c Ts....
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T, typename... Ts>
class index_of
    : public conditional_t<
          index_of_detail::type_index_of_v<T, Ts...> == sizeof...(Ts),
          index_of_detail::empty_index_of_base,
          std::integral_constant<std::size_t, index_of_detail::type_index_of_v<T, Ts...>>> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Template meta-function for querying the 0-based index of 'T' within
/// a list of types, 'Ts...'.
///
/// The instantiation will be ill-formed if T is not a member of Ts...
///
/// @tparam T The type to search for.
/// @tparam Ts The list of types to search in.
template <typename T, typename... Ts>
extern constexpr auto index_of_v = index_of<T, Ts...>::value;

/// @brief Template meta-function for querying the last 0-based index of a type in a list of types.
///
/// @tparam T The type to search for.
/// @tparam Ts The list of types to search in.
///
/// If @c T is found within @c Ts... then this type inherits from <c>std::integral_constant<std::size_t, Idx></c> where
/// @c Idx is the zero-based index of the last occurrence of @c T in @c Ts...
///
/// Access the result using the @c value member of this type. This type will not have a @c value member if @c T is not
/// one of the types listed in @c Ts....
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T, typename... Ts>
class last_index_of
    : public conditional_t<
          index_of_detail::last_type_index_of_v<T, Ts...> == sizeof...(Ts),
          index_of_detail::empty_index_of_base,
          std::integral_constant<std::size_t, index_of_detail::last_type_index_of_v<T, Ts...>>> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Template meta-function for querying the last 0-based index of 'T' within a list of types, 'Ts...'.
///
/// The instantiation will be ill-formed if T is not a member of Ts...
///
/// @tparam T The type to search for.
/// @tparam Ts The list of types to search in.
template <typename T, typename... Ts>
extern constexpr auto last_index_of_v = last_index_of<T, Ts...>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_INDEX_OF_HPP_
