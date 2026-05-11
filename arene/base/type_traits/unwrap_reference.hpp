// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::unwrap_reference"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_UNWRAP_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_UNWRAP_REFERENCE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/reference_wrapper_fwd.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"

namespace arene {
namespace base {
namespace unwrap_reference_detail {

/// @brief obtain the reference type of a std::reference_wrapper
/// @tparam ReferenceWrapper specialization of std::reference_wrapper
///
/// Intended for use as a template alias argument to @c conditional_apply_t.
template <class ReferenceWrapper>
using reference_type_t = typename ReferenceWrapper::type&;

}  // namespace unwrap_reference_detail

/// @brief Backport of @c std::unwrap_reference_t from C++20
/// @tparam T A type from which to remove a @c std::reference_wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
using unwrap_reference_t = conditional_apply_t<  //
    is_instantiation_of_v<T, std::reference_wrapper>,
    unwrap_reference_detail::reference_type_t,
    conditional_identity_t,
    T>;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of std::unwrap_reference from C++20
/// @tparam T A type from which to remove a @c std::reference_wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
struct unwrap_reference {
  /// @brief The result type
  using type = unwrap_reference_t<T>;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of @c std::unwrap_ref_decay_t from C++20
/// @tparam T A type to decay and then unwrap a reference wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
using unwrap_ref_decay_t = unwrap_reference_t<std::decay_t<T>>;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of @c std::unwrap_ref_decay from C++20
/// @tparam T A type to decay and then unwrap a reference wrapper
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T>
class unwrap_ref_decay {
 public:
  /// @brief The decayed type @c T , subsequently unwrapped if applicable
  using type = unwrap_ref_decay_t<T>;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_UNWRAP_REFERENCE_HPP_
