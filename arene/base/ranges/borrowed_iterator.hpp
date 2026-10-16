// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_BORROWED_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_BORROWED_ITERATOR_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/ranges/begin_end.hpp"
#include "arene/base/ranges/dangling.hpp"
#include "arene/base/ranges/is_borrowed_range.hpp"
#include "arene/base/type_traits/conditional.hpp"

namespace arene {
namespace base {
namespace ranges {

namespace borrowed_iterator_detail {

/// @brief Alias template to @c dangling for the fixed result of the non-borrowed branch.
/// @tparam Rng The unused type parameter required by @c conditional_apply_t .
template <typename Rng>
using always_dangling = ::arene::base::ranges::dangling;

}  // namespace borrowed_iterator_detail

/// @brief A type alias conditionally selects between a ranges iterator or @c arene::base::dangling .
/// @tparam Rng The range type for which to obtain the borrowed iterator type.
///
/// If @c Rng satisfies @c arene::base::is_borrowed_range_v<Rng>, the type alias resolves to
/// @c arene::base::ranges::iterator_t<Rng> . Otherwise, it resolves to @c arene::base::ranges::dangling . This can be
/// used to safely create range algorithms which should match all range types, but only return valid content if the
/// input is a borrowed range.
template <typename Rng>
using borrowed_iterator_t =
    conditional_apply_t<is_borrowed_range_v<Rng>, iterator_t, borrowed_iterator_detail::always_dangling, Rng>;

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_BORROWED_ITERATOR_HPP_
