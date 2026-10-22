// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_COND_VALUE_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_COND_VALUE_TYPE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"

namespace arene {
namespace base {
namespace iterator_detail {

/// @brief An exposition-only type trait that conditionally provides a member typedef @c type if @c T is an object.
/// @tparam T The type to inspect.
/// The default specialization does not provide a @c type member.
template <typename T, bool = std::is_object<T>::value>
class cond_value_type {};

/// @brief An exposition-only type trait that conditionally provides a @c type member if @c T is an object.
/// @tparam T The type to inspect.
/// The specialization for a type which is an object provides a @c type member that is @c std::remove_cv_t<T>
template <typename T>
class cond_value_type<T, true> {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'value_type' does not hide anything"
  /// @brief The value type corresponding to @c T if @c T is an object.
  using value_type = std::remove_cv_t<T>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
};

}  // namespace iterator_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_COND_VALUE_TYPE_HPP_
