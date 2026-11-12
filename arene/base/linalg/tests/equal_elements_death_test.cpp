// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {
/// @test If the elements match when calling @c equal_elements with @c mdspan instantiations with different extents, the
/// result is a precondition violation
TEST(EqualElementsDeathTests, MismatchedSpansIsPreconditionViolation) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 3U)> const data1{};

  arene::base::array<std::uint32_t, 3> const extents1{5, 6, 2};
  arene::base::array<std::uint32_t, 3> const extents2{5, 6, 3};
  span1_t const span1{data1.data(), extents1};
  span2_t const span2{data1.data(), extents2};

  ASSERT_DEATH(arene::base::linalg::equal_elements(span1, span2), "Precondition");
}
}  // namespace
