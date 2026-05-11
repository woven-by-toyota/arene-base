// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/tests/test_index_types.hpp"

namespace {

template <typename IndexType>
class ExtentsTypedDeathTest : public testing::Test {};

TYPED_TEST_SUITE(ExtentsTypedDeathTest, test::index_types, );

/// @test Calling @c static_extent with a rank that is out of range is a precondition error
TYPED_TEST(ExtentsTypedDeathTest, OutOfRangeValueForStaticExtentIsPreconditionFailurePart1) {
  using ext1 = arene::base::extents<TypeParam, 1, 3, 6, 7, 9, 10, 11, 12, 13>;

  ASSERT_DEATH(ext1::static_extent(9), "Precondition");
  ASSERT_DEATH(ext1::static_extent(28), "Precondition");

  using ext2 = arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 6, 7, 9, 10, 11, 12, 13>;

  ASSERT_DEATH(ext2::static_extent(9), "Precondition");
  ASSERT_DEATH(ext2::static_extent(28), "Precondition");

  using ext3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  ASSERT_DEATH(ext3::static_extent(14), "Precondition");
  ASSERT_DEATH(ext3::static_extent(28), "Precondition");

  using ext4 = arene::base::extents<TypeParam>;

  ASSERT_DEATH(ext4::static_extent(0), "Precondition");

  using ext5 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  ASSERT_DEATH(ext5::static_extent(6), "Precondition");

  using ext6 = arene::base::extents<
      TypeParam,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  ASSERT_DEATH(ext6::static_extent(6), "Precondition");

  using ext7 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      40,
      arene::base::dynamic_extent,
      6>;

  ASSERT_DEATH(ext7::static_extent(6), "Precondition");

  using ext8 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      41,
      arene::base::dynamic_extent,
      6>;

  ASSERT_DEATH(ext8::static_extent(6), "Precondition");

  using ext9 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  ASSERT_DEATH(ext9::static_extent(6), "Precondition");

  using ext10 = arene::base::extents<TypeParam, 1, 2, 30, 41, 50, 6>;

  ASSERT_DEATH(ext10::static_extent(6), "Precondition");

  using ext11 = arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent>;

  ASSERT_DEATH(ext11::static_extent(6), "Precondition");

  using ext12 = arene::base::extents<TypeParam, 1, 2, 3, 40, 50, 6>;

  ASSERT_DEATH(ext12::static_extent(6), "Precondition");

  using ext13 = arene::base::extents<TypeParam, 100, 101, 102, 103, 104>;

  ASSERT_DEATH(ext13::static_extent(6), "Precondition");

  using ext14 = arene::base::extents<TypeParam, 1, 2, 30, 40, 50, 3>;

  ASSERT_DEATH(ext14::static_extent(6), "Precondition");

  using ext15 = arene::base::extents<TypeParam, 1, 2, 30, 40, 51, 6>;

  ASSERT_DEATH(ext15::static_extent(6), "Precondition");

  using ext16 = arene::base::extents<TypeParam, 1, 2, 31, 40, 50, 6>;

  ASSERT_DEATH(ext16::static_extent(6), "Precondition");

  using ext17 = arene::base::extents<TypeParam, 1, 2, 30, 41, 50>;

  ASSERT_DEATH(ext17::static_extent(6), "Precondition");

  using ext18 = arene::base::extents<TypeParam, 1, 2, 30, 41, 50, 3>;

  ASSERT_DEATH(ext18::static_extent(6), "Precondition");

  using ext19 = arene::base::extents<TypeParam, 1, 2, 30, 41, 51, 6>;

  ASSERT_DEATH(ext19::static_extent(6), "Precondition");

  using ext20 = arene::base::extents<TypeParam, 1, 2, 31, 41, 50, 6>;

  ASSERT_DEATH(ext20::static_extent(6), "Precondition");

  using ext21 = arene::base::extents<TypeParam, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  ASSERT_DEATH(ext21::static_extent(2), "Precondition");

  using ext22 = arene::base::extents<TypeParam, 3>;

  ASSERT_DEATH(ext22::static_extent(2), "Precondition");

  using ext23 = arene::base::extents<TypeParam, 3, 4>;

  ASSERT_DEATH(ext23::static_extent(2), "Precondition");

  using ext24 = arene::base::extents<TypeParam, 3, 4, 5>;

  ASSERT_DEATH(ext24::static_extent(3), "Precondition");

  using ext25 = arene::base::extents<TypeParam, arene::base::dynamic_extent, 4, 5>;

  ASSERT_DEATH(ext25::static_extent(3), "Precondition");
}

}  // namespace
