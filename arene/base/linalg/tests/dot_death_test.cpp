// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/linalg/dot.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"

namespace {

using arene::base::linalg::dot;
using arene::base::linalg_testing::test_array;

template <typename T>
class LinalgDotDeathTest : public ::testing::Test {};

using unqualified_vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    arene::base::mdspan<double const, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<std::uint32_t const, arene::base::extents<std::size_t, 4>, arene::base::layout_left>,
    arene::base::mdspan<
        std::int32_t,
        arene::base::extents<std::size_t, 1>,
        arene::base::layout_right,
        arene::base::linalg_testing::doubled_accessor<std::int32_t>>,
    // Mdspans that are input/output vectors
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<std::uint32_t, arene::base::extents<std::size_t, 4>, arene::base::layout_left>>;

TYPED_TEST_SUITE(LinalgDotDeathTest, unqualified_vector_types, );

/// @test The dot crashes with a precondition violation when the runtime sizes of the two vectors don't match
TYPED_TEST(LinalgDotDeathTest, DotWithSizeMismatchCrashes) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec{zero_test_vec};

  using dyn_vec_type = arene::base::mdspan<typename TypeParam::value_type const, arene::base::dextents<std::size_t, 1>>;

  // A dynamic vector with size matching that of the type parameter does *not* crash.

  arene::base::array<typename TypeParam::value_type, TypeParam::extents_type::static_extent(0)> const dyn_vec_data{};
  dyn_vec_type const dyn_vec{dyn_vec_data.data(), dyn_vec_data.size()};

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests are precise enough to use exact equality in some cases");

  ASSERT_EQ(dot(zero_vec, dyn_vec), typename TypeParam::value_type{});
  ASSERT_EQ(dot(dyn_vec, zero_vec), typename TypeParam::value_type{});

  ARENE_IGNORE_END();

  // A dynamic vector with size 0 *does* crash.

  arene::base::array<typename TypeParam::value_type, 1U> const empty_vec_data{};
  dyn_vec_type const empty_vec{empty_vec_data.data(), 0U};

  ASSERT_DEATH(std::ignore = dot(zero_vec, empty_vec), "Precondition");
  ASSERT_DEATH(std::ignore = dot(empty_vec, zero_vec), "Precondition");
}

}  // namespace
