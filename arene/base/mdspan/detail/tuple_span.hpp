// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_TUPLE_SPAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_TUPLE_SPAN_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief Helper class to make span "tuple-ish"
/// @tparam T Element type
/// @tparam N Number of elements
/// @note @c N must not equal @c dynamic_extent
///
/// Wrapper around a static-sized @c span that implements the tuple protocol.
/// Allows use with tuple-protocol utilities such as @c arene::base::apply.
///
template <class T, std::size_t N>
class tuple_span {
  static_assert(N != dynamic_extent, "'N' must be a static extent value");

  /// @brief underlying span
  span<T, N> value_;

 public:
  /// @brief construct a tuple_span from a span
  /// @param arg The span to adapt
  constexpr explicit tuple_span(span<T, N> arg) noexcept
      : value_{arg} {}

  /// @brief Access underlying span element
  /// @tparam I index of element to access
  /// @return Reference to I-th element of the underlying span
  template <std::size_t I, constraints<std::enable_if_t<(I < N)>> = nullptr>
  constexpr auto get() const noexcept -> typename span<T, N>::reference {
    return value_[I];
  }
};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

/// @brief std::tuple_size specialization for arene::base::mdspan_detail::tuple_span
/// @tparam T Element type
/// @tparam N Number of elements
template <class T, std::size_t N>
class std::tuple_size<arene::base::mdspan_detail::tuple_span<T, N>> : public std::integral_constant<std::size_t, N> {};

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_TUPLE_SPAN_HPP_
