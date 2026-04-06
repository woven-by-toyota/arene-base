// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_UNWRAP_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_UNWRAP_REFERENCE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/reference_wrapper_fwd.hpp"

namespace arene {
namespace base {

/// @brief Backport of std::unwrap_reference from C++20
/// @tparam T A type from which to remove a @c std::reference_wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
struct unwrap_reference {
  /// @brief The type @c T itself, which is not a @c std::reference_wrapper
  using type = T;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of @c std::unwrap_reference from C++20
/// @tparam T A type from which to remove a @c std::reference_wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
struct unwrap_reference<std::reference_wrapper<T>> {
  /// @brief The reference type wrapped by the passed @c std::reference_wrapper
  using type = T&;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of @c std::unwrap_reference_t from C++20
/// @tparam T A type from which to remove a @c std::reference_wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
using unwrap_reference_t = typename unwrap_reference<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of @c std::unwrap_ref_decay from C++20
/// @tparam T A type to decay and then unwrap a reference wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
struct unwrap_ref_decay {
  /// @brief The decayed type @c T , subsequently unwrapped if applicable
  using type = unwrap_reference_t<std::decay_t<T>>;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of @c std::unwrap_ref_decay_t from C++20
/// @tparam T A type to decay and then unwrap a reference wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_UNWRAP_REFERENCE_HPP_
