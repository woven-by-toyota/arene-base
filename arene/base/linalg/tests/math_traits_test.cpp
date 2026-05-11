// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/math_traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

template <typename T>
class LinalgMathTraitsTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgMathTraitsTest, arene::base::linalg_testing::mdspan_types, );

/// @test The @c _t typedefs should be SFINAE-friendly and should work for exactly those types which are linalg objects
TYPED_TEST(LinalgMathTraitsTest, MathTraitsOnlySucceedForLinalgObjects) {
  using arene::base::substitution_succeeds;
  using arene::base::linalg_testing::category;
  using type = typename TypeParam::type;

  constexpr bool should_be_linalg_object{
      (TypeParam::expectation == category::in_vector) || (TypeParam::expectation == category::inout_vector) ||
      (TypeParam::expectation == category::in_matrix) || (TypeParam::expectation == category::inout_matrix)
  };

  constexpr bool has_element_t{substitution_succeeds<arene::base::linalg::math_element_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_element_t);

  constexpr bool has_value_t{substitution_succeeds<arene::base::linalg::math_value_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_value_t);

  constexpr bool has_reference_t{substitution_succeeds<arene::base::linalg::math_reference_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_reference_t);

  constexpr bool has_index_t{substitution_succeeds<arene::base::linalg::math_index_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_index_t);

  constexpr bool has_size_t{substitution_succeeds<arene::base::linalg::math_size_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_size_t);

  constexpr bool has_layout_t{substitution_succeeds<arene::base::linalg::math_layout_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_layout_t);

  constexpr bool has_rank_t{substitution_succeeds<arene::base::linalg::math_rank_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_rank_t);

  constexpr bool has_extents_t{substitution_succeeds<arene::base::linalg::math_extents_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_extents_t);

  constexpr bool has_accessor_t{substitution_succeeds<arene::base::linalg::math_accessor_t, type>};
  STATIC_ASSERT_EQ(should_be_linalg_object, has_accessor_t);
}

template <typename T>
class LinalgMathTraitsCorrectnessTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgMathTraitsCorrectnessTest, arene::base::linalg_testing::linalg_object_types, );

/// @test For linalg object types, the @c _t typedefs should refer to the right types
TYPED_TEST(LinalgMathTraitsCorrectnessTest, TypedefsHaveCorrectValues) {
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_element_t<TypeParam>, typename TypeParam::element_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_value_t<TypeParam>, typename TypeParam::value_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_reference_t<TypeParam>, typename TypeParam::reference>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_index_t<TypeParam>, typename TypeParam::index_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_size_t<TypeParam>, typename TypeParam::size_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_layout_t<TypeParam>, typename TypeParam::layout_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_rank_t<TypeParam>, typename TypeParam::rank_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_extents_t<TypeParam>, typename TypeParam::extents_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_accessor_t<TypeParam>, typename TypeParam::accessor_type>();
}

/// @test For linalg object types, the @c _t typedefs should be the same as accessing the @c math_traits object
TYPED_TEST(LinalgMathTraitsCorrectnessTest, TypedefsAreSameAsUsingMathTraits) {
  using traits = arene::base::linalg::math_traits<TypeParam>;

  ::testing::StaticAssertTypeEq<arene::base::linalg::math_element_t<TypeParam>, typename traits::element_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_value_t<TypeParam>, typename traits::value_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_reference_t<TypeParam>, typename traits::reference>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_index_t<TypeParam>, typename traits::index_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_size_t<TypeParam>, typename traits::size_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_layout_t<TypeParam>, typename traits::layout_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_rank_t<TypeParam>, typename traits::rank_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_extents_t<TypeParam>, typename traits::extents_type>();
  ::testing::StaticAssertTypeEq<arene::base::linalg::math_accessor_t<TypeParam>, typename traits::accessor_type>();
}

}  // namespace
