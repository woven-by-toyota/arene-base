// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UNDERLYING_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UNDERLYING_TYPE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_enum.hpp"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
namespace underlying_type_detail {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

/// @brief Helper trait used to implement @c std::underlying_type
/// @tparam T The type to get the underlying type of
///
/// The primary template does not define member @c type.
///
template <typename T, typename = arene::base::constraints<>>
struct underlying_type {};

/// @brief Helper trait specialization used to implement @c std::underlying_type
///     if @c T is an enumeration type
/// @tparam T The type to get the underlying type of
///
template <typename T>
struct underlying_type<T, arene::base::constraints<enable_if_t<is_enum_v<T>>>> {
  /// @brief The underlying integer type
  using type = __underlying_type(T);
};

}  // namespace underlying_type_detail

/// @brief Obtains the underlying integer type for a given enumeration type
/// @tparam T Enumeration type
///
/// Provides member type alias @c type that names the underlying type of @c T if
/// @c T is an enumeration type. If @c T is not an enumeration type, there is no
/// member @c type.
///
template <typename T>
using underlying_type = underlying_type_detail::underlying_type<T>;

/// @brief An alias for the underlying type of @c T, if @c T is an enumeration type
/// @tparam T The type to get the underlying type of
///
template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UNDERLYING_TYPE_HPP_
