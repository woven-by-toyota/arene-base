// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_THROWING_SLICE_CONVERSIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_THROWING_SLICE_CONVERSIONS_HPP_

#include "arene/base/mdspan/full_extent.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief Test helper with a throwing implicit conversion to @c full_extent_t
struct throwing_convertible_to_full_extent {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator arene::base::full_extent_t() const noexcept(false) { return arene::base::full_extent; }
};

/// @brief Test helper with a throwing implicit conversion to @c int
struct throwing_convertible_to_int {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator int() const noexcept(false) { return 0; }
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_THROWING_SLICE_CONVERSIONS_HPP_
