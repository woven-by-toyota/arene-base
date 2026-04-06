// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

using arene::base::mdspan_detail::representable_cast;

/// @test casting a value ordered after @c numeric_limits<int8_t>::max() to
/// @c int8_t with @c representable_cast is a precondition violation
TEST(MdspanDetailRepresentableCastDeathTest, ValueAfterMax) {
  ASSERT_DEATH(representable_cast<std::int8_t>(std::uint8_t{255}), "Precondition");
  ASSERT_DEATH(representable_cast<std::int8_t>(std::int16_t{255}), "Precondition");
}

/// @test casting a value ordered before @c numeric_limits<int8_t>::max() to
/// @c int8_t with @c representable_cast is a precondition violation
TEST(MdspanDetailRepresentableCastDeathTest, ValueBeforeMin) {
  ASSERT_DEATH(representable_cast<std::int8_t>(std::int16_t{-129}), "Precondition");
}

/// @test casting a negative value to an unsigned integer type is a precondition
/// violation
TEST(MdspanDetailRepresentableCastDeathTest, NegativeToUnsigned) {
  ASSERT_DEATH(representable_cast<std::uint64_t>(std::int16_t{-129}), "Precondition");
}

}  // namespace
