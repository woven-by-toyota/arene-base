// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace {

template <typename IndexType>
class ExtentsTypedDeathTest : public testing::Test {};

TYPED_TEST_SUITE(ExtentsTypedDeathTest, test::index_types, );

template <typename T, typename U>
constexpr auto truncate_to_in_range(U value) -> std::size_t {
  if (value < U{}) {
    return {};
  }

  if (arene::base::cmp_greater(value, std::numeric_limits<std::size_t>::max())) {
    return std::numeric_limits<std::size_t>::max();
  }

  if (arene::base::cmp_greater(value, std::numeric_limits<T>::max())) {
    return static_cast<std::size_t>(std::numeric_limits<T>::max());
  }

  return static_cast<std::size_t>(value);
}

/// @test Calling @c static_extent with a rank that is out of range is a precondition error
TYPED_TEST(ExtentsTypedDeathTest, OutOfRangeValueForStaticExtentIsPreconditionFailurePart2) {
  using ext26 = arene::base::extents<TypeParam, 3, 4, arene::base::dynamic_extent>;

  ASSERT_DEATH(ext26::static_extent(3), "Precondition");

  using ext27 = arene::base::extents<TypeParam, 3, arene::base::dynamic_extent, arene::base::dynamic_extent, 5>;

  ASSERT_DEATH(ext27::static_extent(4), "Precondition");

  using ext28 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  ASSERT_DEATH(ext28::static_extent(5), "Precondition");

  using ext29 = arene::base::
      extents<TypeParam, arene::base::dynamic_extent, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  ASSERT_DEATH(ext29::static_extent(5), "Precondition");

  using ext30 = arene::base::extents<TypeParam, 0>;

  ASSERT_DEATH(ext30::static_extent(5), "Precondition");

  using ext31 = arene::base::extents<TypeParam, 99, arene::base::dynamic_extent>;

  ASSERT_DEATH(ext31::static_extent(5), "Precondition");

  using ext32 = arene::base::extents<TypeParam, 0, arene::base::dynamic_extent>;

  ASSERT_DEATH(ext32::static_extent(5), "Precondition");

  using ext33 = arene::base::extents<TypeParam, 6, 1, 8>;

  ASSERT_DEATH(ext33::static_extent(5), "Precondition");

  using ext34 = arene::base::extents<TypeParam, 0, arene::base::dynamic_extent, 0>;

  ASSERT_DEATH(ext34::static_extent(5), "Precondition");

  using ext35 = arene::base::extents<TypeParam, 9, 28, 0, 22>;

  ASSERT_DEATH(ext35::static_extent(5), "Precondition");

  using ext36 = arene::base::extents<TypeParam, arene::base::dynamic_extent, 6, 1, 8>;

  ASSERT_DEATH(ext36::static_extent(5), "Precondition");

  using ext37 = arene::base::extents<TypeParam, 14, 4, 2, 87, 3>;

  ASSERT_DEATH(ext37::static_extent(6), "Precondition");

  using ext38 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      truncate_to_in_range<TypeParam>(980),
      1,
      arene::base::dynamic_extent,
      4>;

  ASSERT_DEATH(ext38::static_extent(5), "Precondition");

  using ext39 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      truncate_to_in_range<TypeParam>(980),
      0,
      arene::base::dynamic_extent,
      4>;

  ASSERT_DEATH(ext39::static_extent(5), "Precondition");

  using ext40 = arene::base::extents<TypeParam, 4, 2, 87, 3>;

  ASSERT_DEATH(ext40::static_extent(6), "Precondition");

  using ext41 = arene::base::extents<TypeParam, 28, 0, 22>;

  ASSERT_DEATH(ext41::static_extent(6), "Precondition");

  using ext42 = arene::base::extents<TypeParam, 1, 8>;

  ASSERT_DEATH(ext42::static_extent(6), "Precondition");

  using ext43 = arene::base::extents<TypeParam, 15, 5, 3, 88, 4>;

  ASSERT_DEATH(ext43::static_extent(6), "Precondition");

  using ext44 = arene::base::extents<TypeParam, 10, 29, 1, 23>;

  ASSERT_DEATH(ext44::static_extent(6), "Precondition");

  using ext45 = arene::base::extents<TypeParam, 7, 2, 9>;

  ASSERT_DEATH(ext45::static_extent(6), "Precondition");

  using ext46 = arene::base::extents<TypeParam, 4>;

  ASSERT_DEATH(ext46::static_extent(6), "Precondition");

  using ext47 = arene::base::extents<TypeParam, 1>;

  ASSERT_DEATH(ext47::static_extent(6), "Precondition");

  using ext48 = arene::base::extents<
      TypeParam,
      42,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      truncate_to_in_range<TypeParam>(1234)>;

  ASSERT_DEATH(ext48::static_extent(6), "Precondition");

  using ext49 = arene::base::extents<
      TypeParam,
      truncate_to_in_range<TypeParam>(std::numeric_limits<TypeParam>::max() - TypeParam{1}),
      truncate_to_in_range<TypeParam>(std::numeric_limits<TypeParam>::max() - TypeParam{1}),
      truncate_to_in_range<TypeParam>(std::numeric_limits<TypeParam>::max() - TypeParam{1})>;

  ASSERT_DEATH(ext49::static_extent(6), "Precondition");
}

}  // namespace
