// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <iterator>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/optional.hpp"
#include "arene/base/strings/inline_string.hpp"

namespace {

using ::arene::base::inline_string;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

/// @test If an `inline_string` is constructed from a `std::string`, then the length of the string is the same as that
/// of the `std::string`, and `c_str` returns a pointer to a NUL-terminated string with the same contents as the source
/// `std::string`, but which is not a pointer into the `std::string`.
TEST(InlineStringCtors, IfInitializedFromStdStringContentsAreTheSame) {
  std::string const std_string("hello");
  inline_string<default_size> const string_initialized(std_string);

  ASSERT_TRUE(!string_initialized.empty());
  ASSERT_EQ(string_initialized.size(), std_string.size());
  ASSERT_NE(string_initialized.c_str(), nullptr);
  ASSERT_NE(string_initialized.c_str(), std_string.c_str());
  ASSERT_EQ(string_initialized.c_str(), std_string);
}

/// @test After copying an `inline_string` from a `std::string`, whether via copy-construction or assignment, the target
/// `inline_string` compares equal to the source.
TEST(InlineStringCtors, CopyingAndAssignmentFromStdString) {
  constexpr auto buffer_size = 12;
  inline_string<buffer_size> const str_1("helloxxxxxxz");
  inline_string<buffer_size> str_2(str_1);

  str_2 = std::string("world");
  ASSERT_NE(str_1, str_2);
  ASSERT_EQ(str_2, "world");
}

/// @test `inline_string` instances can be compared with `std::string` objects, with the expected results for strings
TEST(InlineStringComparisonOperator, StringComparisonsStdStrings) {
  inline_string<default_size> const inline_str_1("hello");
  constexpr inline_string<default_size> inline_str_2_const("goodbye");
  std::string const std_string_1("hello");
  std::string const std_string_3("hell");
  std::string const std_string_4("world");
  std::string const std_string_2("goodbye");

  ASSERT_EQ(std_string_1, inline_str_1);
  ASSERT_EQ(inline_str_1, std_string_1);
  ASSERT_NE(inline_str_1, std_string_4);
  ASSERT_NE(std_string_4, inline_str_1);
  ASSERT_NE(std_string_1, inline_str_2_const);
  ASSERT_NE(inline_str_2_const, std_string_1);
  ASSERT_EQ(inline_str_2_const, std_string_2);
  ASSERT_EQ(std_string_2, inline_str_2_const);
  ASSERT_NE(inline_str_2_const, std_string_3);

  ASSERT_TRUE(inline_str_1 == "hello");
  ASSERT_TRUE(inline_str_1 != "goodbye");

  ASSERT_EQ(inline_str_2_const, "goodbye");
  ASSERT_NE(inline_str_2_const, "hello");
  ASSERT_LT(inline_str_2_const, "hello");
  ASSERT_LE(inline_str_2_const, "hello");
  ASSERT_GT(inline_str_2_const, "bad");
  ASSERT_GE(inline_str_2_const, "bad");

  ASSERT_LT(inline_str_1, std_string_4);
  ASSERT_LT(std_string_2, inline_str_1);
  ASSERT_LT(inline_str_2_const, std_string_3);
  ASSERT_LE(inline_str_1, std_string_4);
  ASSERT_LE(std_string_2, inline_str_1);
  ASSERT_LE(inline_str_2_const, std_string_3);

  ASSERT_GT(std_string_4, inline_str_1);
  ASSERT_GT(inline_str_1, std_string_2);
  ASSERT_GT(std_string_3, inline_str_2_const);
  ASSERT_GE(std_string_4, inline_str_1);
  ASSERT_GE(inline_str_1, std_string_2);
  ASSERT_GE(std_string_3, inline_str_2_const);
}

/// @test Comparison of `inline_string` objects holding UTF-8 characters is consistent with the results of comparing
/// `std::string` instances with the same contents
TEST(InlineStringComparisonOperator, StringComparisonsWithMixOfHighAndLowCharacters) {
  constexpr auto csize = 5;
  arene::base::array<char, csize> const array_1 = {static_cast<char>(0xff), 0x1, 0x2, 0x3, 0x4};
  std::string const const_std_string_1(array_1.begin(), array_1.end());
  arene::base::array<char, csize> const array_2 = {static_cast<char>(0x7f), 0x1, 0x2, 0x3, 0x4};
  std::string const const_std_string_2(array_2.begin(), array_2.end());

  inline_string<csize> const inline_str_1(const_std_string_1);
  inline_string<csize> const inline_str_2(const_std_string_2);

  ASSERT_EQ((inline_str_1 < inline_str_2), (const_std_string_1 < const_std_string_2));
  ASSERT_EQ((inline_str_1 < inline_str_2), (const_std_string_1 < const_std_string_2));
  ASSERT_EQ((inline_str_1 < inline_str_2.c_str()), (const_std_string_1 < const_std_string_2));
  ASSERT_EQ((inline_str_1.c_str() < inline_str_2), (const_std_string_1 < const_std_string_2));
}

/// @test An `inline_string` constructed from a `std::string` with embedded NUL characters has the full size and
/// contents of the original string, and the characters after the embedded NUL character are still included in string
/// comparisons.
TEST(InlineString, EmbeddedNullsThroughString) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const cstring_with_nulls[] = {'h', 'e', 'l', 'l', 'o', '\0', 'w', 'o', 'r', 'l', 'd'};
  std::string std_str(&cstring_with_nulls[0], sizeof(cstring_with_nulls));

  inline_string<sizeof(cstring_with_nulls)> const inline_str(std_str);

  ASSERT_EQ(inline_str.size(), sizeof(cstring_with_nulls));
  ASSERT_NE(inline_str, &cstring_with_nulls[0]);
  ASSERT_EQ(inline_str, std_str);

  std_str.back() = 'z';
  ASSERT_NE(inline_str, std_str);
  ASSERT_LT(inline_str, std_str);
}

/// @test Invoking `assign` with a `std::string` sets the destination string to be equal to the `std::string`
TEST(InlineString, AssignmentFromStdString) {
  std::string const std_string{"hello"};
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign(std_string);
  ASSERT_EQ(inline_str, "hello");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

/// @test `try_construct` returns an empty optional if passed an input iterator with a size larger than the capacity
TEST(InlineStringTryConstruct, FromInputIteratorReturnsNulloptIfDistanceGreaterThanCapacity) {
  SCOPED_TRACE("InputIterator");
  std::stringstream too_large;
  too_large << std::string(default_size + 1, 'f');
  EXPECT_EQ(
      inline_string<default_size>::try_construct(std::istream_iterator<char>(too_large), std::istream_iterator<char>()),
      arene::base::nullopt
  );
}

/// @test `try_construct` from an input iterator with an in-bounds size returns a string with the expected contents
TEST(InlineStringTryConstruct, FromInputIteratorsReturnsStringIfSizeLessThanOrEqalCapacity) {
  SCOPED_TRACE("InputIterator");
  std::string const original_exactly_capacity(default_size, 'f');
  std::stringstream exactly_capacity;
  exactly_capacity << original_exactly_capacity;
  EXPECT_EQ(
      inline_string<default_size>::try_construct(
          std::istream_iterator<char>(exactly_capacity),
          std::istream_iterator<char>()
      ),
      inline_string<default_size>{original_exactly_capacity}
  );

  std::string const original_smaller_capacity{"abc"};
  std::stringstream smaller_capacity;
  smaller_capacity << original_smaller_capacity;
  EXPECT_EQ(
      inline_string<default_size>::try_construct(
          std::istream_iterator<char>(smaller_capacity),
          std::istream_iterator<char>()
      ),
      inline_string<default_size>{original_smaller_capacity}
  );
}

}  // namespace
