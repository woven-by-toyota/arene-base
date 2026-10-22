// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/utility/alignment.hpp"

namespace {

using integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<bool, unsigned char, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>>;

template <typename T>
struct BitMaskDeathTest : public ::testing::Test {
  static constexpr std::uint32_t max_bits = sizeof(T) * CHAR_BIT;
};

TYPED_TEST_SUITE(BitMaskDeathTest, integer_types, );
/// @test Invoking @c arene::base::bit_mask<Integer>(one_bits) with @c one_bits larger than the number of bits in the
/// representation of @c Integer is an @c ARENE_PRECONDITION violation.
TYPED_TEST(BitMaskDeathTest, IfBitsIsGreaterThanNumberOfBitsInIntegerRepresentationItIsAPreconditionViolation) {
  ASSERT_DEATH(std::ignore = ::arene::base::bit_mask<TypeParam>(TestFixture::max_bits + 1), "Precondition");
}
}  // namespace
