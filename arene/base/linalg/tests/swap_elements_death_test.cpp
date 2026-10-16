// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/swap_elements.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {
/// @test Calling @c swap_elements with @c mdspan instantiations with different extents is a precondition violation
TEST(SwapElementsDeathTest, MismatchedSpansIsPreconditionViolation) {
  using left_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using right_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 3U)> left_data{};
  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 3U)> right_data{};

  arene::base::array<std::uint32_t, 3> const left_extents{5, 6, 2};
  arene::base::array<std::uint32_t, 3> const right_extents{5, 6, 3};
  left_span_t const left{left_data.data(), left_extents};
  right_span_t const right{right_data.data(), right_extents};

  ASSERT_DEATH(arene::base::linalg::swap_elements(left, right), "Precondition");
}
}  // namespace
