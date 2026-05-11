// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdexcept>

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::inline_string;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

/// @test Invoking `at` on a `const` `inline_string` with an index that is less than or equal to the size of the string
/// is equivalent to invoking `operator[]`
CONSTEXPR_TEST(InlineStringAt, ForInRangeIsEquivalentToIndexOperatorConst) {
  constexpr inline_string<default_size> str{"hello"};
  CONSTEXPR_ASSERT_EQ(str.at(0), str[0]);
  CONSTEXPR_ASSERT_EQ(str.at(2), str[2]);
  CONSTEXPR_ASSERT_EQ(str.at(str.size() - 1), str[str.size() - 1]);
}

/// @test Invoking `at` on a non-`const` `inline_string` with an index that is less than or equal to the size of the
/// string is equivalent to invoking `operator[]`
CONSTEXPR_TEST(InlineStringAt, ForInRangeIsEquivalentToIndexOperatorNonConst) {
  inline_string<default_size> str{"hello"};
  str.at(0) = 'j';
  str.at(4) = 'y';
  CONSTEXPR_ASSERT_EQ(str, "jelly");
}

/// @test Invoking `at` with an index that is less larger than the size of the string throws `std::out_of_range`
TEST(InlineStringAt, ForOutOfRangeThrows) {
  inline_string<default_size> str{"hello"};
  ASSERT_THROW(str.at(str.size() + 1) = '0', std::out_of_range);
  auto const& const_str = str;
  ASSERT_THROW(std::ignore = const_str.at(str.size() + 1), std::out_of_range);
}

}  // namespace
