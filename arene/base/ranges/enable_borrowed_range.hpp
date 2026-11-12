// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_ENABLE_BORROWED_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_ENABLE_BORROWED_RANGE_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {
namespace ranges {

/// @brief An opt-in trait to indicate that a range type is a borrowed range.
///
/// @tparam Rng The range type to check.
///
/// A borrowed range is a range whose iterators remain valid even after the range object is destroyed: for example
/// @c arene::base::span and @c arene::base::string_view are borrowed ranges, while @c arene::base::inline_vector and
/// @c arene::base::inline_string are not.
///
/// Users may specialize @c enable_borrowed_range for their range types to indicate that they are borrowed ranges by
/// providing a static @c value member of type @c bool which indicates if the range type is a borrowed range or not
/// (typically by inheriting from @c std::true_type if it is unconditional). The default specialization is @c false .
///
/// \snippet docs/examples/subpackages/ranges/type_traits_examples.cpp specialize_borrowed_range
template <typename Rng>
class enable_borrowed_range : public std::false_type {};

template <typename Rng>
/// @brief Query if a range type has opted into being a borrowed range.
///
/// @tparam Rng The range type to check.
/// @return bool Equivalent to @c enable_borrowed_range<Rng>::value .
/// @note Users should generally not query @c enable_borrowed_range_v directly; it is intended for use by the range
/// library to detect @c enable_borrowed_range customizations, and should only be used for that purpose. Instead, use
/// @c arene::base::is_borrowed_range_v<Rng> to query if a type qualifies as a borrowed range when implementing generic
/// algorithms.
extern constexpr bool enable_borrowed_range_v = enable_borrowed_range<Rng>::value;

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_ENABLE_BORROWED_RANGE_HPP_
