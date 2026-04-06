// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_DENOTES_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_DENOTES_RANGE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {

/// Check if an iterator/sentinel pair form an iterable range. The value is @c true if the pair do form an iterable
/// range, @c false otherwise.
/// @tparam Iterator The type to check as an iterator
/// @tparam Sentinel The type to check as a sentinel
template <typename Iterator, typename Sentinel = Iterator>
extern constexpr bool denotes_range_v =
    is_iterator_v<remove_cvref_t<Iterator>> &&  //
    is_inequality_comparable_v<remove_cvref_t<Iterator>, remove_cvref_t<Sentinel>> &&
    std::is_copy_constructible<remove_cvref_t<Sentinel>>::value;

namespace denotes_range_detail {
/// @brief implementation helper for @c arene::base::denotes_nothrow_iterable_range_v .
/// This specialization handles the case that the iterator/sentinal pair do not form an iterable range
/// @tparam Iterator The type to check as an iterator
/// @tparam Sentinel The type to check as a sentinel
template <typename Iterator, typename Sentinel, typename = constraints<>>
extern constexpr bool denotes_nothrow_iterable_range_v = false;

/// @brief implementation helper for @c arene::base::denotes_nothrow_iterable_range_v .
/// This specialization handles the case that the iterator/sentinal pair do form an iterable range
/// @tparam Iterator The type to check as an iterator
/// @tparam Sentinel The type to check as a sentinel
template <typename Iterator, typename Sentinel>
extern constexpr bool denotes_nothrow_iterable_range_v<
    Iterator,
    Sentinel,
    constraints<std::enable_if_t<denotes_range_v<Iterator, Sentinel>>>> =
    noexcept(++std::declval<remove_cvref_t<Iterator>&>())&&  //
    noexcept(std::declval<remove_cvref_t<Iterator>&>()++)&&  //
    noexcept(*std::declval<remove_cvref_t<Iterator>&>())&&   //
    noexcept(std::declval<remove_cvref_t<Iterator>&>() != std::declval<remove_cvref_t<Sentinel>&>());

}  // namespace denotes_range_detail

/// @brief Check if an iterator/sentinel pair form an iterable range that can be iterated without throwing.
///
/// @tparam Iterator The type to check as an iterator
/// @tparam Sentinel The type to check as a sentinel
/// @return true If the pair satisfies @c denotes_iterable_range and which cant be iterated without throwing: all of
///         @c operator++ , @c operator++(int) , @c operator* , and @c operator!= are @c noexcept .
/// @return false otherwise.
template <typename Iterator, typename Sentinel = Iterator>
extern constexpr bool denotes_nothrow_iterable_range_v =
    denotes_range_detail::denotes_nothrow_iterable_range_v<Iterator, Sentinel>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_DENOTES_RANGE_HPP_
