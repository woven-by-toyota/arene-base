// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_ZERO_OBJECT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_ZERO_OBJECT_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/linalg/constant.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/stdlib_choice/declval.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief type of the linear-algebra object, with the extents of @c InObject, whose every entry is zero
/// @tparam InObject linear-algebra object type whose value type and extents type determine the zero object
template <typename InObject>
using zero_object_t =
    decltype(constant(numbers::zero_v<math_value_t<InObject>>, std::declval<typename InObject::extents_type const&>()));

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief create a linear-algebra object whose every entry is zero
/// @tparam InObject linear-algebra object type whose value type and extents type determine the zero object
/// @param exts extents of the zero object
/// @return an object of extents @c exts, each of whose entries is <c>zero_v<math_value_t<InObject>></c>
template <typename InObject>
constexpr auto zero_object(typename InObject::extents_type const& exts
) noexcept(noexcept(constant(numbers::zero_v<math_value_t<InObject>>, exts))) -> zero_object_t<InObject> {
  static_assert(is_in_object_v<InObject>, "InObject must be a linear-algebra object");
  return constant(numbers::zero_v<math_value_t<InObject>>, exts);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace detail
}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_ZERO_OBJECT_HPP_
