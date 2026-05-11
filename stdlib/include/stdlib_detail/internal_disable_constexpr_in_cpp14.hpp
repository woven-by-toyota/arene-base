// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_DISABLE_CONSTEXPR_IN_CPP14_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_DISABLE_CONSTEXPR_IN_CPP14_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

namespace std {
namespace detail {

/// @brief function used to disable @c constexpr in C++14 but allow for it in C++17
/// @tparam T dummy template parameter used to avoid warnings that a
///     @c constexpr function never produces a constant expression
///
template <class T = void>
constexpr auto disable_constexpr_in_cpp14() noexcept -> void {
  []() {}();
}

}  // namespace detail
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_DISABLE_CONSTEXPR_IN_CPP14_HPP_
