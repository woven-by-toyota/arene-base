// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_IS_BORROWED_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_IS_BORROWED_RANGE_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/ranges/enable_borrowed_range.hpp"
#include "arene/base/ranges/is_range.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {

/// @brief Predicate to check if a type is a borrowed range.
/// @tparam Rng The type to check.
/// @return bool Equivalent to @c is_borrowed_range<Rng>::value .
template <typename Rng>
extern constexpr bool is_borrowed_range_v{
    is_range_v<Rng> && (std::is_lvalue_reference<Rng>::value || enable_borrowed_range_v<remove_cvref_t<Rng>>)
};

/// @brief Trait which determins if a type is a borrowed range.
/// @tparam Rng The type to check.
///
/// A borrowed range is a range whose iterators remain valid even after the lifetime of the reference to the range
/// object ends, and is thus safe to use as an input to range-based algorithms which return iterators or other
/// references to the content in the range.
///
/// A type is considered a borrowed range if and only if both of the following conditions are true:
/// 1. @c is_range<Rng> is satisfied
/// 2. Either @c std::is_lvalue_reference<Rng> is satisfied or @c enable_borrowed_range<Rng> is specialized
///    to be @c true
template <typename Rng>
class is_borrowed_range : public std::integral_constant<bool, is_borrowed_range_v<Rng>> {};

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_IS_BORROWED_RANGE_HPP_
