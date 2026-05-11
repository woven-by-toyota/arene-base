// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/integer_sequences.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"  // IWYU pragma: keep

namespace {

using integer_types = ::testing::Types<
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t,
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t>;

template <typename Integer>
class MakeIntegerSequenceTest : public ::testing::Test {};

TYPED_TEST_SUITE(MakeIntegerSequenceTest, integer_types, );

/// @test `make_integer_sequence_from` with Begin=0 and Size creates a sequence [0, Size)
TYPED_TEST(MakeIntegerSequenceTest, BeginZeroCreatesSequenceFromZero) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, 0, 5>,
      std::integer_sequence<TypeParam, 0, 1, 2, 3, 4>>();
}

/// @test `make_integer_sequence_from` with Begin>0 creates a sequence starting at Begin
TYPED_TEST(MakeIntegerSequenceTest, NonZeroBeginCreatesOffsetSequence) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, 5, 3>,
      std::integer_sequence<TypeParam, 5, 6, 7>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, 10, 4>,
      std::integer_sequence<TypeParam, 10, 11, 12, 13>>();

  constexpr auto max = std::numeric_limits<TypeParam>::max();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, max, 1>,
      std::integer_sequence<TypeParam, max>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, max - 2, 3>,
      std::integer_sequence<TypeParam, max - 2, max - 1, max>>();
}

/// @test `make_integer_sequence_from` with Begin<0 creates a sequence starting at Begin
CONDITIONAL_TYPED_TEST(MakeIntegerSequenceTest, NegativeBeginCreatesOffsetSequence, std::is_signed<TypeParam>::value) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, -5, 3>,
      std::integer_sequence<TypeParam, -5, -4, -3>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, -10, 4>,
      std::integer_sequence<TypeParam, -10, -9, -8, -7>>();

  constexpr auto min = std::numeric_limits<TypeParam>::min();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, min, 1>,
      std::integer_sequence<TypeParam, min>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, min, 3>,
      std::integer_sequence<TypeParam, min, min + 1, min + 2>>();
}

/// @test `make_integer_sequence_from` with Size=0 creates an empty sequence
TYPED_TEST(MakeIntegerSequenceTest, SizeZeroCreatesEmptySequence) {
  ::testing::
      StaticAssertTypeEq<arene::base::make_integer_sequence_from<TypeParam, 0, 0>, std::integer_sequence<TypeParam>>();
  ::testing::
      StaticAssertTypeEq<arene::base::make_integer_sequence_from<TypeParam, 10, 0>, std::integer_sequence<TypeParam>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_from<TypeParam, std::numeric_limits<TypeParam>::max(), 0>,
      std::integer_sequence<TypeParam>>();
}

/// @test `make_index_sequence_from` is a convenience alias for make_integer_sequence_from<size_t, Begin, Size>
TEST(MakeIndexSequenceTest, IsConvenienceAliasForSizeT) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_index_sequence_from<0, 0>,
      arene::base::make_integer_sequence_from<std::size_t, 0, 0>>();

  ::testing::StaticAssertTypeEq<
      arene::base::make_index_sequence_from<0, 5>,
      arene::base::make_integer_sequence_from<std::size_t, 0, 5>>();

  ::testing::StaticAssertTypeEq<
      arene::base::make_index_sequence_from<10, 5>,
      arene::base::make_integer_sequence_from<std::size_t, 10, 5>>();
}

/// @test `make_integer_sequence_between` with Begin=0 and End creates a sequence [0, End)
TYPED_TEST(MakeIntegerSequenceTest, FromBeginZeroCreatesSequenceFromZero) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, 0, 5>,
      std::integer_sequence<TypeParam, 0, 1, 2, 3, 4>>();
}

/// @test `make_integer_sequence_between` with Begin>0 creates a sequence [Begin, End)
TYPED_TEST(MakeIntegerSequenceTest, FromNonZeroBeginCreatesRangeSequence) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, 5, 8>,
      std::integer_sequence<TypeParam, 5, 6, 7>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, 10, 14>,
      std::integer_sequence<TypeParam, 10, 11, 12, 13>>();

  constexpr auto max = std::numeric_limits<TypeParam>::max();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, max - 1, max>,
      std::integer_sequence<TypeParam, max - 1>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, max - 3, max>,
      std::integer_sequence<TypeParam, max - 3, max - 2, max - 1>>();
}

/// @test `make_integer_sequence_between` with Begin<0 creates a sequence [Begin, End)
CONDITIONAL_TYPED_TEST(
    MakeIntegerSequenceTest,
    FromNegativeBeginCreatesRangeSequence,
    std::is_signed<TypeParam>::value
) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, -8, -5>,
      std::integer_sequence<TypeParam, -8, -7, -6>>();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, -3, 3>,
      std::integer_sequence<TypeParam, -3, -2, -1, 0, 1, 2>>();

  constexpr auto min = std::numeric_limits<TypeParam>::min();
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, min, min + 1>,
      std::integer_sequence<TypeParam, min>>();

  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, min, min + 3>,
      std::integer_sequence<TypeParam, min, min + 1, min + 2>>();
}

/// @test `make_integer_sequence_between` with Begin=End creates an empty sequence
TYPED_TEST(MakeIntegerSequenceTest, FromBeginEqualEndCreatesEmptySequence) {
  ::testing::
      StaticAssertTypeEq<arene::base::make_integer_sequence_between<TypeParam, 0, 0>, std::integer_sequence<TypeParam>>(
      );
  ::testing::StaticAssertTypeEq<
      arene::base::make_integer_sequence_between<TypeParam, 10, 10>,
      std::integer_sequence<TypeParam>>();
}

/// @test `make_index_sequence_between` is a convenience alias for make_integer_sequence_between<size_t, Begin, End>
TEST(MakeIndexSequenceTest, FromIsConvenienceAliasForSizeT) {
  ::testing::StaticAssertTypeEq<
      arene::base::make_index_sequence_between<0, 0>,
      arene::base::make_integer_sequence_between<std::size_t, 0, 0>>();

  ::testing::StaticAssertTypeEq<
      arene::base::make_index_sequence_between<0, 5>,
      arene::base::make_integer_sequence_between<std::size_t, 0, 5>>();

  ::testing::StaticAssertTypeEq<
      arene::base::make_index_sequence_between<10, 15>,
      arene::base::make_integer_sequence_between<std::size_t, 10, 15>>();
}

}  // namespace
