// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/object_traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::linalg_testing::category;

template <typename T>
class LinalgTraitsTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgTraitsTest, arene::base::linalg_testing::mdspan_types, );

/// @test All of the test types are <c>mdspan</c>s except for the scalars and the nothings
TYPED_TEST(LinalgTraitsTest, IsMdspan) {
  STATIC_ASSERT_EQ(
      arene::base::is_mdspan_v<typename TypeParam::type>,
      (TypeParam::expectation != category::nothing) && (TypeParam::expectation != category::scalar)
  );
}

/// @test Scalars are copyable, default-initializable types which are neither mdspans nor execution policies
TYPED_TEST(LinalgTraitsTest, IsScalar) {
  STATIC_ASSERT_EQ(
      arene::base::linalg::is_scalar_v<typename TypeParam::type>,
      (TypeParam::expectation == category::scalar)
  );
}

/// @test <c>mdspan</c>s are vectors when they're rank 1, and they're out or inout when the element type is movable
TYPED_TEST(LinalgTraitsTest, IsVector) {
  // Sanity check for expectations: vectors should all be rank 1.
  STATIC_ASSERT_FALSE(
      ((TypeParam::expectation == category::in_vector) || (TypeParam::expectation == category::inout_vector)) &&
      arene::base::linalg_testing::rank_v<typename TypeParam::type> != 1U
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_in_vector_v<typename TypeParam::type>,
      ((TypeParam::expectation == category::in_vector) || (TypeParam::expectation == category::inout_vector))
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_out_vector_v<typename TypeParam::type>,
      (TypeParam::expectation == category::inout_vector)
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_inout_vector_v<typename TypeParam::type>,
      (TypeParam::expectation == category::inout_vector)
  );
}

/// @test <c>mdspan</c>s are matrices when they're rank 2, and they're out or inout when the element type is movable
TYPED_TEST(LinalgTraitsTest, IsMatrix) {
  // Sanity check for expectations: matrices should all be rank 2.
  STATIC_ASSERT_FALSE(
      ((TypeParam::expectation == category::in_matrix) || (TypeParam::expectation == category::inout_matrix)) &&
      arene::base::linalg_testing::rank_v<typename TypeParam::type> != 2U
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_in_matrix_v<typename TypeParam::type>,
      ((TypeParam::expectation == category::in_matrix) || (TypeParam::expectation == category::inout_matrix))
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_out_matrix_v<typename TypeParam::type>,
      (TypeParam::expectation == category::inout_matrix)
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_inout_matrix_v<typename TypeParam::type>,
      (TypeParam::expectation == category::inout_matrix)
  );
}

/// @test <c>mdspan</c>s are linalg objects when they're rank 1 or 2, and they're out/inout when the element is movable
TYPED_TEST(LinalgTraitsTest, IsLinalgObject) {
  STATIC_ASSERT_EQ(
      arene::base::linalg::is_in_object_v<typename TypeParam::type>,
      ((TypeParam::expectation == category::in_vector) || (TypeParam::expectation == category::inout_vector) ||
       (TypeParam::expectation == category::in_matrix) || (TypeParam::expectation == category::inout_matrix))
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_out_object_v<typename TypeParam::type>,
      ((TypeParam::expectation == category::inout_vector) || (TypeParam::expectation == category::inout_matrix))
  );

  STATIC_ASSERT_EQ(
      arene::base::linalg::is_inout_object_v<typename TypeParam::type>,
      ((TypeParam::expectation == category::inout_vector) || (TypeParam::expectation == category::inout_matrix))
  );
}

}  // namespace
