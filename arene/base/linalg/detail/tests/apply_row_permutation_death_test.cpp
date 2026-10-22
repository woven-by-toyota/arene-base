// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/linalg/detail/apply_row_permutation.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

// GTest and mdspan supply their own implementation dependencies.
// IWYU pragma: no_include <string>
// IWYU pragma: no_include "arene/base/stdlib_choice/move.hpp"

namespace {
template <typename T>
class ApplyRowPermutationDeathTest : public ::testing::Test {};

using object_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 3, 2>>>;
TYPED_TEST_SUITE(ApplyRowPermutationDeathTest, object_types, );

/// @test Given incompatible dynamic dimensions, when permuting rows, then the dimension precondition fails.
TYPED_TEST(ApplyRowPermutationDeathTest, MismatchedDimensionsArePreconditionViolations) {
  arene::base::linalg_testing::test_array<TypeParam, true> storage{};
  auto object = storage.mdspan();
  arene::base::array<int, 16> permutation_storage{};
  using shape = arene::base::dextents<std::size_t, 2>;
  // Exercise each dimension separately as well as square permutations of the wrong size.
  arene::base::array<shape, 4> const dimensions{{shape{2, 3}, shape{3, 2}, shape{2, 2}, shape{4, 4}}};
  for (auto const& dimension : dimensions) {
    arene::base::mdspan<int, shape> const permutation{permutation_storage.data(), dimension};
    ASSERT_DEATH(arene::base::linalg::detail::apply_row_permutation(permutation, object), "Precondition");
  }
}
}  // namespace
