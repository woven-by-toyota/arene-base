// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/alignment.hpp"

#include <gtest/gtest.h>

#include "arene/base/byte/byte.hpp"
#include "arene/base/math/log2.hpp"
#include "arene/base/math/power_of_2.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/for_each.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace {
/// @test @c arene::base::alignment_offset<1>(ptr) returns @c 0 for all @c ptr .
TEST(AlignmentOffset, OffsetForAlignmentOf1AlwaysZero) {
  constexpr auto size = 10;
  // NOLINTBEGIN(hicpp-avoid-c-arrays) Cannot depend on arene::base::array here.
  unsigned char array1[size] = {};
  unsigned array2[size] = {};
  // NOLINTEND(hicpp-avoid-c-arrays)

  for (auto& element : array1) {
    ASSERT_EQ(arene::base::alignment_offset<1>(&element), 0);
  }
  for (auto& element : array2) {
    ASSERT_EQ(arene::base::alignment_offset<1>(&element), 0);
  }
}
/// @test @c arene::base::alignment_offset<2>(ptr) returns @c 0 for content which is aligned to 2 bytes, and 1 for
/// content which is not.
TEST(AlignmentOffset, OffsetForAlignmentOf2MayBe0or1) {
  constexpr auto size = 10;
  union AlignedTo2 {
    char c[2];  // NOLINT
    short s;    // NOLINT
  };

  static_assert(alignof(AlignedTo2) == 2, "Check that AlignedTo2 has intended alignment");

  // NOLINTBEGIN(hicpp-avoid-c-arrays) Cannot depend on arene::base::array here.
  unsigned char normal_aligned_values[size] = {};
  AlignedTo2 abnormal_aligned_values[size] = {};
  // NOLINTEND(hicpp-avoid-c-arrays)

  // NOLINTNEXTLINE(hicpp-no-array-decay)
  for (auto& element : normal_aligned_values) {
    auto const offset = arene::base::alignment_offset<2>(&element);
    ASSERT_TRUE(offset == 0 || offset == 1);
  }
  // NOLINTNEXTLINE(hicpp-no-array-decay)
  for (auto& element : abnormal_aligned_values) {
    ASSERT_TRUE(arene::base::alignment_offset<2>(&element) == 0);
    ASSERT_TRUE(arene::base::alignment_offset<2>(&element.c[0]) == 0);  // NOLINT
    ASSERT_TRUE(arene::base::alignment_offset<2>(&element.c[1]) == 1);  // NOLINT
  }
}

/// Get the offsets for p for each of the specified alignments
template <std::size_t... Alignments>
auto get_alignment_offsets(void const* ptr, std::index_sequence<Alignments...>) {
  return std::make_tuple(arene::base::alignment_offset<Alignments>(ptr)...);
}

/// Check that the alignment offsets are in range for each entry
template <typename Offsets, std::size_t... Alignments, std::size_t... Indices>
auto offsets_in_range(Offsets const& offsets, std::index_sequence<Alignments...>, std::index_sequence<Indices...>)
    -> bool {
  auto const check_results = std::make_tuple((std::get<Indices>(offsets) <= Alignments)...);
  bool res = true;
  arene::base::for_each(check_results, [&](bool val) {
    if (!val) {
      res = false;
    }
  });
  return res;
}

/// Adjust each offset to one less than it was, wrapping round from 0 to
/// Alignment-1
template <typename Offsets, std::size_t... Alignments, std::size_t... Indices>
void adjust_offsets(Offsets& offsets, std::index_sequence<Alignments...>, std::index_sequence<Indices...>) {
  static_cast<void>(
      std::make_tuple((std::get<Indices>(offsets) = (std::get<Indices>(offsets) + Alignments - 1) % Alignments)...)
  );
}

/// @test @c arene::base::alignment_offset<Alignment>(ptr) 's return is a constant value for all content of a given
/// alignment.
TEST(AlignmentOffset, OffsetForAlignmentIsConsistent) {
  constexpr auto max_align = alignof(std::max_align_t);
  constexpr auto size = max_align * 2;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) Cannot depend on arene::base::array here.
  unsigned char array1[size] = {};

  constexpr auto alignments = arene::base::make_power_of_2_sequence<1, arene::base::log2(max_align) + 1>();

  constexpr auto indices = std::make_index_sequence<arene::base::log2(max_align)>();

  auto offsets = get_alignment_offsets(&array1, alignments);

  for (auto& element : array1) {
    auto const new_offsets = get_alignment_offsets(&element, alignments);
    ASSERT_EQ(new_offsets, offsets);
    ASSERT_TRUE(offsets_in_range(new_offsets, alignments, indices));
    adjust_offsets(offsets, alignments, indices);
  }
}

/// @test @c arene::base::is_aligned<Alignment>(uint8_t) returns @c true when @c value is an even power of @c Alignment
/// , for all power-of-2 @c Alignment .
TEST(IsAligned, uint8) {
  // Exhaustively check all possible combinations for uint8_t
  for (auto value = std::numeric_limits<std::uint8_t>::lowest(); value < std::numeric_limits<std::uint8_t>::max();
       ++value) {
    ASSERT_TRUE(arene::base::is_aligned<1>(value));
    ASSERT_EQ(arene::base::is_aligned<2>(value), ((value % 2) == 0));
    ASSERT_EQ(arene::base::is_aligned<4>(value), ((value % 4) == 0));
    ASSERT_EQ(arene::base::is_aligned<8>(value), ((value % 8) == 0));
    ASSERT_EQ(arene::base::is_aligned<16>(value), ((value % 16) == 0));
    ASSERT_EQ(arene::base::is_aligned<32>(value), ((value % 32) == 0));
    ASSERT_EQ(arene::base::is_aligned<64>(value), ((value % 64) == 0));
  }
}
/// @test @c arene::base::is_aligned<Alignment>(int8_t) returns @c true when @c value is an even power of @c Alignment
/// , for all power-of-2 @c Alignment .
TEST(IsAligned, int8) {
  // Exhaustively check all possible combinations for int8_t
  for (auto value = std::numeric_limits<std::int8_t>::lowest(); value < std::numeric_limits<std::int8_t>::max();
       ++value) {
    ASSERT_TRUE(arene::base::is_aligned<1>(value));
    ASSERT_EQ(arene::base::is_aligned<2>(value), ((value % 2) == 0));
    ASSERT_EQ(arene::base::is_aligned<4>(value), ((value % 4) == 0));
    ASSERT_EQ(arene::base::is_aligned<8>(value), ((value % 8) == 0));
    ASSERT_EQ(arene::base::is_aligned<16>(value), ((value % 16) == 0));
    ASSERT_EQ(arene::base::is_aligned<32>(value), ((value % 32) == 0));
    ASSERT_EQ(arene::base::is_aligned<64>(value), ((value % 64) == 0));
  }
}
/// @test Given a power-of-2 @c Alignment , @c arene::base::is_aligned<Alignment>(uint32_t) returns @c true when
/// @c value is an even power of @c Alignment .
TEST(IsAligned, uint32) {
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint32_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<2>(std::uint32_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<8>(std::uint32_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<512>(std::uint32_t(0)));

  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint32_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint32_t(8)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint32_t(60)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint32_t(4294967295)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<2>(std::uint32_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<2>(std::uint32_t(8)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<2>(std::uint32_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<2>(std::uint32_t(4294967295)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<4>(std::uint32_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<4>(std::uint32_t(8)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<4>(std::uint32_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<4>(std::uint32_t(4294967295)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<8>(std::uint32_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<8>(std::uint32_t(8)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<8>(std::uint32_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<8>(std::uint32_t(4294967295)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint32_t(1)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint32_t(8)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint32_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint32_t(4294967295)));
}
/// @test Given a power-of-2 @c Alignment , @c arene::base::is_aligned<Alignment>(uint64_t) returns @c true when
/// @c value is an even power of @c Alignment .
TEST(IsAligned, uint64) {
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint64_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<2>(std::uint64_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<8>(std::uint64_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<512>(std::uint64_t(0)));

  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint64_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint64_t(8)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint64_t(60)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<1>(std::uint64_t(18446744073709551615ULL)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<2>(std::uint64_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<2>(std::uint64_t(8)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<2>(std::uint64_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<2>(std::uint64_t(18446744073709551615ULL)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<4>(std::uint64_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<4>(std::uint64_t(8)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<4>(std::uint64_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<4>(std::uint64_t(18446744073709551615ULL)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<8>(std::uint64_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_aligned<8>(std::uint64_t(8)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<8>(std::uint64_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<8>(std::uint64_t(18446744073709551615ULL)));

  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint64_t(1)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint64_t(8)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint64_t(60)));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<16>(std::uint64_t(18446744073709551615ULL)));

  STATIC_ASSERT_TRUE(arene::base::is_aligned<65536>(std::uint64_t(12345678) * 65536));
  STATIC_ASSERT_FALSE(arene::base::is_aligned<65536>(std::uint64_t(12345678) * 65536 + 1));
}

/// @test Given a power-of-2 @c Alignment, @c arene::base::is_aligned<Alignment>(ptr) returns @c true if the address
/// pointed to is an even power of @c Alignment.
TEST(IsAligned, Pointer) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) Cannot depend on arene::base::array here.
  alignas(16) const arene::base::byte buffer[32] = {};
  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_TRUE(arene::base::is_aligned<1>(&buffer[0]));
  ASSERT_TRUE(arene::base::is_aligned<1>(&buffer[1]));
  ASSERT_TRUE(arene::base::is_aligned<1>(&buffer[2]));

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_TRUE(arene::base::is_aligned<2>(&buffer[0]));
  ASSERT_FALSE(arene::base::is_aligned<2>(&buffer[1]));
  ASSERT_TRUE(arene::base::is_aligned<2>(&buffer[2]));
  ASSERT_FALSE(arene::base::is_aligned<2>(&buffer[3]));

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_TRUE(arene::base::is_aligned<4>(&buffer[0]));
  ASSERT_FALSE(arene::base::is_aligned<4>(&buffer[1]));
  ASSERT_FALSE(arene::base::is_aligned<4>(&buffer[2]));
  ASSERT_FALSE(arene::base::is_aligned<4>(&buffer[3]));
  ASSERT_TRUE(arene::base::is_aligned<4>(&buffer[4]));

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_TRUE(arene::base::is_aligned<8>(&buffer[0]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[1]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[2]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[3]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[4]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[5]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[6]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[7]));
  ASSERT_TRUE(arene::base::is_aligned<8>(&buffer[8]));
  ASSERT_FALSE(arene::base::is_aligned<8>(&buffer[9]));

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_TRUE(arene::base::is_aligned<16>(&buffer[0]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[1]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[2]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[3]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[4]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[5]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[6]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[7]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[8]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[9]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[10]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[11]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[12]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[13]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[14]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[15]));
  ASSERT_TRUE(arene::base::is_aligned<16>(&buffer[16]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[17]));
  ASSERT_FALSE(arene::base::is_aligned<16>(&buffer[31]));
}
/// @test Given a power-of-2 @c Alignment, @c arene::base::align_floor<Alignment>(uint32_t) returns the greatest common
/// multiple of @c value and @c Alignment which is not greater than @c value .
TEST(AlignFloor, uint32) {
  STATIC_ASSERT_EQ(arene::base::align_floor<1>(std::uint32_t(0)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<1>(std::uint32_t(1)), std::uint32_t(1));
  STATIC_ASSERT_EQ(arene::base::align_floor<1>(std::uint32_t(2)), std::uint32_t(2));

  STATIC_ASSERT_EQ(arene::base::align_floor<2>(std::uint32_t(0)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<2>(std::uint32_t(1)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<2>(std::uint32_t(2)), std::uint32_t(2));
  STATIC_ASSERT_EQ(arene::base::align_floor<2>(std::uint32_t(3)), std::uint32_t(2));
  STATIC_ASSERT_EQ(arene::base::align_floor<2>(std::uint32_t(4)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_floor<2>(std::uint32_t(5)), std::uint32_t(4));

  STATIC_ASSERT_EQ(arene::base::align_floor<4>(std::uint32_t(0)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<4>(std::uint32_t(1)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<4>(std::uint32_t(2)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<4>(std::uint32_t(3)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_floor<4>(std::uint32_t(4)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_floor<4>(std::uint32_t(5)), std::uint32_t(4));
}
/// @test Given a power-of-2 @c Alignment, @c arene::base::align_floor<Alignment>(uint32_t) returns the least common
/// multiple of @c value and @c Alignment which is greater than @c value .
TEST(AlignCeil, uint32) {
  STATIC_ASSERT_EQ(arene::base::align_ceil<1>(std::uint32_t(0)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_ceil<1>(std::uint32_t(1)), std::uint32_t(1));
  STATIC_ASSERT_EQ(arene::base::align_ceil<1>(std::uint32_t(2)), std::uint32_t(2));

  STATIC_ASSERT_EQ(arene::base::align_ceil<2>(std::uint32_t(0)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_ceil<2>(std::uint32_t(1)), std::uint32_t(2));
  STATIC_ASSERT_EQ(arene::base::align_ceil<2>(std::uint32_t(2)), std::uint32_t(2));
  STATIC_ASSERT_EQ(arene::base::align_ceil<2>(std::uint32_t(3)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_ceil<2>(std::uint32_t(4)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_ceil<2>(std::uint32_t(5)), std::uint32_t(6));

  STATIC_ASSERT_EQ(arene::base::align_ceil<4>(std::uint32_t(0)), std::uint32_t(0));
  STATIC_ASSERT_EQ(arene::base::align_ceil<4>(std::uint32_t(1)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_ceil<4>(std::uint32_t(2)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_ceil<4>(std::uint32_t(3)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_ceil<4>(std::uint32_t(4)), std::uint32_t(4));
  STATIC_ASSERT_EQ(arene::base::align_ceil<4>(std::uint32_t(5)), std::uint32_t(8));
}

// std::uint8_t may be the same as unsigned char on some platforms
using integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<bool, unsigned char, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>>;

template <typename T>
struct BitMaskTest : public ::testing::Test {
  static constexpr std::uint32_t max_bits = sizeof(T) * CHAR_BIT;
};

TYPED_TEST_SUITE(BitMaskTest, integer_types, );

/// @test The return type of @c arene::base::bit_mask<Integer>() is @c Integer .
TYPED_TEST(BitMaskTest, ReturnTypeIsTemplateType) {
  ::testing::StaticAssertTypeEq<decltype(::arene::base::bit_mask<TypeParam>(0U)), TypeParam>();
}
/// @test Given a number of bits, @c arene::base::bit_mask<Integer>(one_bits) returns a value of type @c Integer which
/// has the lowest @c one_bits bits set to @c 1 and the rest set to @c 0 . Mathematically, this value is equivalent to
/// @c pow(2,one_bits)-1 .
TYPED_TEST(BitMaskTest, ReturnValueIsInstanceOfIntegerWithLowestBitsBitsSetTo1AndTheRestSetTo0) {
  for (std::uint32_t bits = 0; bits <= this->max_bits; ++bits) {
    auto const expected =
        bits == TestFixture::max_bits
            ? std::numeric_limits<TypeParam>::max()
            : static_cast<TypeParam>((static_cast<TypeParam>(1U) << bits) - static_cast<TypeParam>(1U));
    ASSERT_EQ(arene::base::bit_mask<TypeParam>(bits), expected);
  }
}

}  // namespace
