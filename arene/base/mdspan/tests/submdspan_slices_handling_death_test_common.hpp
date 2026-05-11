#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_SUBMDSPAN_SLICES_HANDLING_DEATH_TEST_COMMON_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_SUBMDSPAN_SLICES_HANDLING_DEATH_TEST_COMMON_HPP_

#include <gtest/gtest.h>

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace {

using arene::base::extent_slice;
using arene::base::extents;
using arene::base::range_slice;

template <class T>
class SubmdspanSliceHandlingCommonDeathTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(SubmdspanSliceHandlingCommonDeathTest);

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when a slice value is not representable as the extents' index type
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, IntegerSliceNotRepresentableAsIndexType) {
  ASSERT_DEATH(TypeParam::function_under_test(256), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice has values that are not representable as the extent's index type
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, ExtentSliceWithNonRepresentableValues) {
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, 256, 1}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice has a positive extent value and a zero stride
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, ExtentSliceWithPositiveExtentAndZeroStride) {
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, 1, 0}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice has negative values
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, ExtentSliceWithNegativeValues) {
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{-1, 0, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{-1, 1, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, -1, 0}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, -1, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, 0, -1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, 1, -1}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice upper bound exceeds the extent
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, ExtentSliceUpperBoundExceedsTheExtent) {
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{4, 0, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{3, 1, 0}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{3, 1, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{3, 2, 1}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice upper bound is not representable as the extents' index type
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, ExtentSliceUpperBoundNotRepresentableAsIndexType) {
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{256, 0, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(extent_slice<int, int, int>{0, 2, 256}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice upper bound "overflows"
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, ExtentSliceUpperBoundOverflows) {
  auto const huge = static_cast<std::size_t>(-1);
  ASSERT_DEATH(
      TypeParam::function_under_test(extent_slice<std::size_t, std::size_t, std::size_t>{huge, 2, huge}),
      "Precondition"
  );
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an @c extent_slice stride value is not representable in the extents'
/// index type, even if the slice upper bound value *would* be representable.
TYPED_TEST_P(
    SubmdspanSliceHandlingCommonDeathTest,
    ExtentSliceWithNonRepresentableValuesButRepresentableSliceUpperBound
) {
  auto const huge = static_cast<std::size_t>(-1);
  ASSERT_DEATH(
      TypeParam::function_under_test(extent_slice<std::size_t, std::size_t, std::size_t>{0, 0, huge}),
      "Precondition"
  );
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when the extent of a @c range_slice is not representable as the extents' index type
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, RangeSliceExtentNotRepresentableAsIndexType) {
  ASSERT_DEATH(TypeParam::function_under_test(range_slice<int, int, int>{0, 256, 1}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when the last value of a @c range_slice is less than the first value of a @c
/// range_slice
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, NegativeRangeSliceInterval) {
  ASSERT_DEATH(TypeParam::function_under_test(range_slice<int, int, int>{1, 0, 1}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when the stride value of a @c range_slice not positive
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, NonPositiveRangeSliceExtent) {
  ASSERT_DEATH(TypeParam::function_under_test(range_slice<int, int, int>{0, 1, 0}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(range_slice<int, int, int>{0, 1, -1}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when the upper bound of a @c range_slice exceeds the extent
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, RangeSliceUpperBoundExceedsTheExtent) {
  ASSERT_DEATH(TypeParam::function_under_test(range_slice<int, int, int>{0, 4, 1}), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(range_slice<int, int, int>{0, 6, 2}), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when a @c range_slice last value is not representable in
/// the extents' index type, even if is slice upper bound value *would* be
/// representable.
TYPED_TEST_P(
    SubmdspanSliceHandlingCommonDeathTest,
    RangeSliceWithNonRepresentableValuesButRepresentableSliceUpperBound
) {
  auto const huge = static_cast<std::size_t>(-1);
  ASSERT_DEATH(
      TypeParam::function_under_test(range_slice<std::size_t, std::size_t, std::size_t>{huge, huge, 1}),
      "Precondition"
  );
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an index slice is negative
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, NegativeIndexSlice) {  //
  ASSERT_DEATH(TypeParam::function_under_test(-1), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an index slice exceeds the extent
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, IndexSliceExceedsExtent) {
  ASSERT_DEATH(TypeParam::function_under_test(3), "Precondition");
  ASSERT_DEATH(TypeParam::function_under_test(4), "Precondition");
}

/// @test @c submdspan_canonicalize_slices and submdspan_extents trigger a precondition violation
/// when an index slice is not representable in the index type
TYPED_TEST_P(SubmdspanSliceHandlingCommonDeathTest, IndexSliceValueNotRepresentable) {
  ASSERT_DEATH(TypeParam::function_under_test(static_cast<std::size_t>(-1)), "Precondition");
}

REGISTER_TYPED_TEST_SUITE_P(
    SubmdspanSliceHandlingCommonDeathTest,
    IntegerSliceNotRepresentableAsIndexType,
    ExtentSliceWithNonRepresentableValues,
    ExtentSliceWithPositiveExtentAndZeroStride,
    ExtentSliceWithNegativeValues,
    ExtentSliceUpperBoundExceedsTheExtent,
    ExtentSliceUpperBoundNotRepresentableAsIndexType,
    ExtentSliceUpperBoundOverflows,
    ExtentSliceWithNonRepresentableValuesButRepresentableSliceUpperBound,
    RangeSliceExtentNotRepresentableAsIndexType,
    NegativeRangeSliceInterval,
    NonPositiveRangeSliceExtent,
    RangeSliceUpperBoundExceedsTheExtent,
    RangeSliceWithNonRepresentableValuesButRepresentableSliceUpperBound,
    NegativeIndexSlice,
    IndexSliceExceedsExtent,
    IndexSliceValueNotRepresentable
);

}  // namespace
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_SUBMDSPAN_SLICES_HANDLING_DEATH_TEST_COMMON_HPP_
