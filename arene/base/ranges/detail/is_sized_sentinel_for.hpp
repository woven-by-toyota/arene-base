#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_SIZED_SENTINEL_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_SIZED_SENTINEL_FOR_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/iter_difference_t.hpp"
#include "arene/base/ranges/detail/is_sentinel_for.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {
namespace detail {

/// @brief Trait modelling @c std::ranges::sized_sentinel_for : @c true when the difference between a sentinel @c S and
/// an iterator @c I can be taken in both directions and each direction yields exactly the iterator's difference type.
///
/// This is deliberately kept in the @c size_detail namespace and omits the @c std::disable_sized_sentinel_for opt-out
/// customization point; it can be promoted to a public trait later if a need arises. The primary template handles the
/// case where the requirements are not met.
/// @tparam Sentinel the sentinel type
/// @tparam Iterator the iterator type
template <typename Sentinel, typename Iterator, typename = constraints<>>
extern constexpr bool is_sized_sentinel_for_v = false;

/// @brief Specialization for the case where the sized-sentinel requirements are met.
/// @tparam Sentinel the sentinel type
/// @tparam Iterator the iterator type
template <typename Sentinel, typename Iterator>
extern constexpr bool is_sized_sentinel_for_v<
    Sentinel,
    Iterator,
    constraints<
        std::enable_if_t<std::is_same<
            decltype(std::declval<Sentinel const&>() - std::declval<Iterator const&>()),
            iter_difference_t<Iterator>>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<Iterator const&>() - std::declval<Sentinel const&>()),
            iter_difference_t<Iterator>>::value>>> = is_sentinel_for_v<Sentinel, Iterator>;

}  // namespace detail
}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_SIZED_SENTINEL_FOR_HPP_
