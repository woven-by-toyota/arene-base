// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

using ::arene::base::from_strong_ordering;
using ::arene::base::opposite_ordering;
using ::arene::base::strong_ordering;

/// @test The program terminates if `opposite_ordering` is called with an out-of-range value.
TEST(OppositeOrderingDeathTest, UnreachableViolationForOutOfRange) {
  ASSERT_DEATH(opposite_ordering(static_cast<strong_ordering>(std::numeric_limits<std::int8_t>::max())), "Unreachable");
}
/// @test The program terminates if `from_strong_ordering` is called with an out-of-range value.
TEST(FromStrongOrderingDeathTest, UnreachableViolationForOutOfRange) {
  ASSERT_DEATH(
      from_strong_ordering(static_cast<strong_ordering>(std::numeric_limits<std::int8_t>::max())),
      "Unreachable"
  );
}

}  // namespace
