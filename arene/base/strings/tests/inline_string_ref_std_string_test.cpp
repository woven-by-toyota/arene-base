// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/inline_string_reference.hpp"

namespace {

using ::arene::base::const_inline_string_reference;
using ::arene::base::inline_string;
using ::arene::base::inline_string_reference;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

// const reference type
using const_ref_t = const_inline_string_reference;
// reference type
using ref_t = inline_string_reference;

/// @test Can assign through an `inline_string_reference` using a `std::string`
TEST(InlineStringRefAssignment, AssignmentFromStdString) {
  std::string const std_string{"hello"};
  inline_string<default_size> inline_str;
  auto ref = ref_t(inline_str);
  ref.assign(std_string);
  ASSERT_EQ(inline_str, "hello");
  ASSERT_EQ(ref.c_str(), std_string);
}

class InlineStringRefComparison : public ::testing::Test {
 protected:
  static constexpr std::size_t capacity = 42;
  static constexpr std::size_t capacity_other = 43;

  static constexpr inline_string<capacity> str_const{"hello"};
  static constexpr inline_string<capacity_other> str_const_other{"world"};
};

constexpr inline_string<InlineStringRefComparison::capacity> InlineStringRefComparison::str_const;
constexpr inline_string<InlineStringRefComparison::capacity_other> InlineStringRefComparison::str_const_other;

/// @test Can compare `const_inline_string_reference` instances to various other types for equality
TEST_F(InlineStringRefComparison, OtherTypeEquality) {
  constexpr const_ref_t const_ref(str_const);
  std::string const std_string_same(str_const.c_str());
  std::string const std_string_other(str_const_other.c_str());

  EXPECT_EQ(std_string_same, const_ref);
  EXPECT_EQ(const_ref, std_string_same);
  EXPECT_NE(const_ref, std_string_other);
  EXPECT_NE(std_string_other, const_ref);
}

/// @test Can compare `const_inline_string_reference` instances to various other types for ordering
TEST_F(InlineStringRefComparison, OtherTypeOrdering) {
  constexpr const_ref_t const_ref(str_const);
  std::string const std_string_other(str_const_other.c_str());
  std::string const std_string_early("goodbye");

  EXPECT_LT(std_string_early, const_ref);
  EXPECT_LT(const_ref, std_string_other);
  EXPECT_LE(std_string_early, const_ref);
  EXPECT_LE(const_ref, std_string_other);

  EXPECT_GT(const_ref, std_string_early);
  EXPECT_GT(std_string_other, const_ref);
  EXPECT_GE(const_ref, std_string_early);
  EXPECT_GE(std_string_other, const_ref);
}

/// @test Can compare `inline_string_reference` instances to various other types
TEST_F(InlineStringRefComparison, MutableComparison) {
  inline_string<default_size> str(str_const);
  ref_t const ref(str);
  std::string const std_string_same(str_const.c_str());
  std::string const std_string_other(str_const_other.c_str());
  std::string const std_string_early("goodbye");

  EXPECT_EQ(std_string_same, ref);
  EXPECT_EQ(ref, std_string_same);
  EXPECT_NE(ref, std_string_other);
  EXPECT_NE(std_string_other, ref);

  EXPECT_LT(ref, std_string_other);
  EXPECT_LT(std_string_early, ref);
  EXPECT_LE(ref, std_string_other);
  EXPECT_LE(std_string_early, ref);

  EXPECT_GT(std_string_other, ref);
  EXPECT_GT(ref, std_string_early);
  EXPECT_GE(std_string_other, ref);
  EXPECT_GE(ref, std_string_early);
}

}  // namespace
