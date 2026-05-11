// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace {

template <typename IndexType>
class ExtentsTypedDeathTest : public testing::Test {};

TYPED_TEST_SUITE(ExtentsTypedDeathTest, test::index_types, );

/// @test Passing negative values is a precondition violation
TYPED_TEST(ExtentsTypedDeathTest, NegativeValuesAreAPreconditionFailure) {
  using extent_type = arene::base::extents<TypeParam, arene::base::dynamic_extent>;

  constexpr auto negative = arene::base::array<std::int8_t, 1>{-1};
  ASSERT_DEATH((extent_type{negative}), "Precondition");
}

/// @test Passing non-representable values is a precondition violation
CONDITIONAL_TYPED_TEST(
    ExtentsTypedDeathTest,
    NonRepresenatbleDynamicValuesAreAPreconditionFailure,
    arene::base::cmp_less(std::numeric_limits<std::uint8_t>::max(), std::numeric_limits<TypeParam>::max())
) {
  using extent_type = arene::base::extents<std::uint8_t, arene::base::dynamic_extent>;

  constexpr auto non_representable = arene::base::array<TypeParam, 1>{TypeParam{256}};
  ASSERT_DEATH((extent_type{non_representable}), "Precondition");
}

/// @test Passing an extent that doesn't match the corresponding fixed extent, or is negative, is a precondition failure
TYPED_TEST(ExtentsTypedDeathTest, InvalidExtentsAreAPreconditionFailure) {
  using ext1 = arene::base::extents<
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

  // NOLINTNEXTLINE(misc-const-correctness)
  std::int32_t mismatch = 99;
  ASSERT_DEATH((ext1{0, mismatch, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}), "Precondition");

  if (std::is_signed<TypeParam>::value) {
    ASSERT_DEATH((ext1{-5, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}), "Precondition");
    ASSERT_DEATH((ext1{-5, 2, 4, 5, 8}), "Precondition");
    ASSERT_DEATH((ext1{0, -1, 4, 5, 8}), "Precondition");
    ASSERT_DEATH((ext1{0, 2, -1, 5, 8}), "Precondition");
    ASSERT_DEATH((ext1{0, 2, 4, -1, 8}), "Precondition");
    ASSERT_DEATH((ext1{0, 2, 4, 5, -1}), "Precondition");
  }
}

/// @test Constructing an @c extents object from another where the source has a dynamic extent and the target has a
/// fixed extent is a precondition error if the actual dynamic value doesn't match the fixed value
TYPED_TEST(
    ExtentsTypedDeathTest,
    ConstructingFromAnotherExtentObjectIsAPreconditionErrorIfSourceDynamicExtentDoesNotMatchTargetFixedExtent
) {
  using source_type1 = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;
  using target_type1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 40;
  constexpr std::uint64_t chosen3 = 50;
  source_type1 const source1{chosen1, chosen2, chosen3};

  ASSERT_DEATH(target_type1{source1}, "Precondition");

  using target_type2 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      chosen2 + 1,
      arene::base::dynamic_extent,
      6>;

  ASSERT_DEATH(target_type2{source1}, "Precondition");

  using target_type3 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2 + 1, chosen3, 6>;

  ASSERT_DEATH(target_type3{source1}, "Precondition");
}

template <typename MaybeLargerType, typename MaybeSmallerType>
extern constexpr bool max_may_overflow_v = static_cast<std::uint64_t>(std::numeric_limits<MaybeSmallerType>::max()) <
                                           static_cast<std::uint64_t>(std::numeric_limits<MaybeLargerType>::max());

template <typename Type>
auto extents_with_large_dynamic_extent() -> arene::base::extents<Type, 1, 2, arene::base::dynamic_extent> {
  return arene::base::extents<Type, 1, 2, arene::base::dynamic_extent>{std::numeric_limits<Type>::max() - 1};
}

/// @test Constructing an @c extents object from another where the source extent exceeds the max value representable in
/// the index type for a dynamic extent is a precondition violation
TYPED_TEST(
    ExtentsTypedDeathTest,
    ConstructingFromAnotherExtentObjectIsAPreconditionErrorIfSourceDynamicExtentExceedsIndexMax
) {
  using target_type = arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent>;
  if (max_may_overflow_v<std::uint64_t, TypeParam>) {
    ASSERT_DEATH(target_type{extents_with_large_dynamic_extent<std::uint64_t>()}, "Precondition");
  }
  if (max_may_overflow_v<int, TypeParam>) {
    ASSERT_DEATH(target_type{extents_with_large_dynamic_extent<int>()}, "Precondition");
  }
  // NOLINTNEXTLINE(google-runtime-int)
  if (max_may_overflow_v<long, TypeParam>) {
    ASSERT_DEATH(target_type{extents_with_large_dynamic_extent<long>()}, "Precondition");
  }
  if (max_may_overflow_v<unsigned, TypeParam>) {
    ASSERT_DEATH(target_type{extents_with_large_dynamic_extent<unsigned>()}, "Precondition");
  }
}

/// @test Constructing an @c extents object from a @c span where the source extent gives a negative value of the index
/// type for a dynamic extent is a precondition violation
TYPED_TEST(ExtentsTypedDeathTest, ConstructingFromASpanIsAPreconditionErrorIfSourceforDynamicIsNegative) {
  if (std::is_signed<TypeParam>::value) {
    using source_type1 = arene::base::span<std::int32_t const, 3>;
    using target_type1 = arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent>;

    arene::base::array<std::int32_t, 3> const source{1, 2, -1};
    ASSERT_DEATH(target_type1{source_type1{source}}, "Precondition");
  }
}

/// @test Constructing an @c extents object from an @c array where the source extent doesn't match a fixed extent is a
/// precondition violation
TYPED_TEST(ExtentsTypedDeathTest, ConstructingFromAnArrayIsAPreconditionErrorIfSourceDoesNotMatchFixedExtent) {
  using target_type1 = arene::base::extents<TypeParam, 1, 2, 3>;

  arene::base::array<TypeParam, 3> const source{1, 2, 4};
  ASSERT_DEATH(target_type1{source}, "Precondition");

  using target_type2 = arene::base::extents<TypeParam, 100, 101, 102, 103, 104>;

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<TypeParam, 5> source2{100, 101, 0, 103, 104};
  ASSERT_DEATH(target_type2{source2}, "Precondition");

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<int, 5> source3{100, 101, 0, 103, 104};
  ASSERT_DEATH(target_type2{source3}, "Precondition");

  using target_type4 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1UL,
      arene::base::dynamic_extent,
      3UL,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6UL,
      7UL,
      arene::base::dynamic_extent,
      9UL,
      10UL,
      11UL,
      12UL,
      13UL>;

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<int, 14> source4{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  ASSERT_DEATH(target_type4{source4}, "Precondition");
}

/// @test Constructing an @c extents object from a @c span where the source extent doesn't match a fixed extent is a
/// precondition violation
TYPED_TEST(ExtentsTypedDeathTest, ConstructingFromASpanIsAPreconditionErrorIfSourceDoesNotMatchFixedExtent) {
  using source_type1 = arene::base::span<TypeParam const, 3>;
  using target_type1 = arene::base::extents<TypeParam, 1, 2, 3>;

  arene::base::array<TypeParam, 3> const source1{1, 2, 4};
  ASSERT_DEATH(target_type1{source_type1{source1}}, "Precondition");

  using source_type2 = arene::base::span<TypeParam const, 5>;
  using target_type2 = arene::base::extents<TypeParam, 100, 101, 102, 103, 104>;

  arene::base::array<TypeParam, 5> source2{100, 101, 0, 103, 104};
  ASSERT_DEATH(target_type2{source_type2{source2}}, "Precondition");

  using source_type3 = arene::base::span<int, 5>;
  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<int, 5> source3{100, 101, 0, 103, 104};
  ASSERT_DEATH(target_type2{source_type3{source3}}, "Precondition");

  using source_type4 = arene::base::span<int, 14>;
  using target_type4 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1UL,
      arene::base::dynamic_extent,
      3UL,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6UL,
      7UL,
      arene::base::dynamic_extent,
      9UL,
      10UL,
      11UL,
      12UL,
      13UL>;

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<int, 14> source4{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  ASSERT_DEATH(target_type4{source_type4{source4}}, "Precondition");
}

/// @test Constructing an @c extents object from another where the source extent exceeds the max value representable in
/// the index type for a dynamic extent is a precondition violation, even for same size types
TYPED_TEST(
    ExtentsTypedDeathTest,
    ConstructingFromAnotherExtentObjectIsAPreconditionErrorIfSourceDynamicExtentExceedsIndexMaxForSameSize
) {
  if (std::is_signed<TypeParam>::value) {
    using unsigned_type = std::make_unsigned_t<TypeParam>;
    using source_type1 = arene::base::extents<unsigned_type, 1, 2, arene::base::dynamic_extent>;
    using target_type1 = arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent>;

    constexpr auto chosen1 =
        static_cast<unsigned_type>(static_cast<unsigned_type>(std::numeric_limits<TypeParam>::max()) + 1U);
    source_type1 const source1{chosen1};

    ASSERT_DEATH(target_type1{source1}, "Precondition");
  }
}

/// @test Calling @c static_extent for any dimension is a precondition violation if there are no dimensions
TYPED_TEST(ExtentsTypedDeathTest, StaticExtentIsAlwaysAPreconditionFailureIfNoExtents) {
  ASSERT_DEATH(arene::base::extents<TypeParam>::static_extent(0), "Precondition");
  ASSERT_DEATH(arene::base::extents<TypeParam>::static_extent(1), "Precondition");
}

/// @test Calling @c make_extents with negative values is a precondition failure
TYPED_TEST(ExtentsTypedDeathTest, MakeExtentsWithNegativeValuesIsPreconditionFailure) {
  ASSERT_DEATH(arene::base::make_extents(int{-1}), "Precondition");
}

/// @test Calling @c make_extents with values not repesentable in 'std::size_t' is a precondition failure
CONDITIONAL_TYPED_TEST(
    ExtentsTypedDeathTest,
    MakeExtentsWithNonRepresentableInSizeTIsPreconditionFailure,
    arene::base::cmp_less(std::numeric_limits<std::size_t>::max(), std::numeric_limits<TypeParam>::max())
) {
  constexpr auto unrepresentable_value = static_cast<TypeParam>(std::numeric_limits<std::size_t>::max()) + TypeParam{1};

  ASSERT_DEATH(arene::base::make_extents(unrepresentable_value), "Precondition");
  ASSERT_DEATH(arene::base::make_extents(std::integral_constant<TypeParam, unrepresentable_value>{}), "Precondition");
}

}  // namespace
