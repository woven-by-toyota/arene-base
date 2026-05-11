// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_TYPE_IDENTITY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_TYPE_IDENTITY_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

/// @brief Polyfill of std::type_identity added in C++20
/// @tparam T The type to preserve
///
/// @details
/// Typically used to prevent parameters from being deduced in order
/// to avoid ambiguities in type-deduction.
///
/// Can also be used as the identity metafunction in some template
/// metaprogramming expressions.
///
/// For example:
/// @snippet docs/examples/type_traits_examples.cpp type_identity_usage_example
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
class type_identity {
 public:
  /// @brief an alias for @c T
  using type = T;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Polyfill of std::type_identity_t added in C++20.
/// @tparam T The type to preserve
/// @see arene::base::type_identity
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
using type_identity_t = typename type_identity<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_TYPE_IDENTITY_HPP_
