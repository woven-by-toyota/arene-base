// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DISABLE_SIZED_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DISABLE_SIZED_RANGE_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/integral_constant.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {

/// @brief An opt-out trait to indicate that a range type's member/ADL @c size() is not a constant-time operation.
///
/// @tparam Rng The range type to check.
///
/// Users should specialize this trait for their range types to derive from @c std::true_type when the type's member or
/// ADL @c size() cannot be assumed to run in constant time. The primary template derives from @c std::false_type .
///
/// When the trait is @c true , @c arene::base::ranges::size ignores any member and ADL @c size() . It may still obtain
/// the size from <c>end() - begin()</c> when the iterator/sentinel pair models a sized sentinel, as that subtraction is
/// assumed to be a constant-time operation.
template <typename Rng>
class disable_sized_range : public std::false_type {};

/// @brief Accessor for the value of @c disable_sized_range .
/// @tparam Rng The range type to check.
/// @return Equivalent to @c disable_sized_range<Rng>::value .
template <typename Rng>
extern constexpr bool disable_sized_range_v{disable_sized_range<Rng>::value};

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DISABLE_SIZED_RANGE_HPP_
