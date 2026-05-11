// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_SWAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_SWAP_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

// parasoft-begin-suppress AUTOSAR-A8_4_8-a "False positive: 'lhs' and 'rhs' are in-out parameters, not output
// parameters"

/// @brief swaps the values of two objects
///
/// @tparam T type of the objects to swap
/// @param lhs object to swap
/// @param rhs object to swap
template <class T, arene::base::constraints<std::enable_if_t<arene::base::is_default_swappable_v<T>>> = nullptr>
constexpr auto swap(T& lhs, T& rhs) noexcept(arene::base::is_nothrow_default_swappable_v<T>) -> void {
  auto temp = std::move(lhs);
  lhs = std::move(rhs);
  rhs = std::move(temp);
}

// parasoft-end-suppress AUTOSAR-A8_4_8-a

/// @brief swaps the contents of two arrays
///
/// @tparam T type of the objects in the arrays to swap
/// @param lhs array to swap
/// @param rhs array to swap
template <class T, std::size_t N, arene::base::constraints<std::enable_if_t<arene::base::is_swappable_v<T>>> = nullptr>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays) explicitly providing c-array support
constexpr auto swap(T (&lhs)[N], T (&rhs)[N]) noexcept(arene::base::is_nothrow_swappable_v<T>) -> void {
  for (std::size_t idx{}; idx < N; ++idx) {
    swap(lhs[idx], rhs[idx]);
  }
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_SWAP_HPP_
