// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_UTILITY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_UTILITY_HPP_

namespace testing {

template <typename T>
auto declval() noexcept -> T&&;

template <typename FloatingPoint>
constexpr auto is_nan(FloatingPoint value) noexcept -> bool {
  // This weird implementation is equivalent to !(value == value); it's written like this to confuse GCC's -Wfloat-equal
  // check without including //arene/base/compiler_support. With optimizations on it generates the same code as ==.

  // NOLINTNEXTLINE(misc-redundant-expression) This is not redundant for floating point types
  return !((value >= value) && (value <= value));
}

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_UTILITY_HPP_
