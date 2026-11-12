// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_IS_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_IS_RANGE_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/ranges/begin_end.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {

namespace is_range_detail {
/// @brief Helper trait for deducing if @c ranges::begin is well-formed for a given type @c Rng .
/// @tparam Rng The type to check.
/// @note This is needed rather than just using @c iterator_t because @c iterator_t automatically corrects the input
/// range type to an lvalue reference, which is not what we want for this trait.
template <typename Rng>
using use_begin = decltype(::arene::base::ranges::begin(std::declval<Rng>()));

/// @brief Helper trait for deducing if @c ranges::end is well-formed for a given type @c Rng .
/// @tparam Rng The type to check.
/// @note This is needed rather than just using @c sentinel_t because @c sentinel_t automatically corrects the input
/// range type to an lvalue reference, which is not what we want for this trait.
template <typename Rng>
using use_end = decltype(::arene::base::ranges::end(std::declval<Rng>()));

}  // namespace is_range_detail

/// @brief Predicate to check if a type is a range.
/// @tparam Rng The type to check.
/// @return bool @c true if @c Rng is a range, @c false otherwise. A type @c Rng is considered a range if and only if
/// all of the following are true when considering an instance @c rng of <c>Rng</c>, with the ref-qualification of
/// @c Rng preserved:
/// 1. @c ranges::begin(rng) is a well-formed expression.
/// 2. @c ranges::end(rng) is a well-formed expression.
template <typename Rng>
extern constexpr bool
    is_range_v{substitution_succeeds<is_range_detail::use_begin, Rng> && substitution_succeeds<is_range_detail::use_end, Rng>};

/// @brief Trait which determins if a type is a range.
/// @tparam Rng The type to check.
/// The member @c value is equivalent to @c is_range_v<Rng> .
template <typename Rng>
class is_range : public std::integral_constant<bool, is_range_v<Rng>> {};

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_IS_RANGE_HPP_
