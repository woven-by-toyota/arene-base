// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/algorithm/trim.hpp"

#include <gtest/gtest.h>

#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::inline_string;
using ::arene::base::ltrim;
using ::arene::base::rtrim;
using ::arene::base::string_view;
using ::arene::base::trim;

template <typename StringType>
class TrimTestBase : public ::testing::Test {};

using string_types = ::testing::Types<string_view, inline_string<123>>;

template <typename StringType>
class LTrimTest : public TrimTestBase<StringType> {};

TYPED_TEST_SUITE(LTrimTest, string_types, );

/// @test `ltrim` returns the input if it contains no leading whitespaces.
TYPED_TEST(LTrimTest, StringWithNoWhiteSpaceReturnsInput) {
  constexpr TypeParam string{"hello world"};
  STATIC_ASSERT_EQ(ltrim(string), string);
}

/// @test `ltrim` trims leading whitespaces.
TYPED_TEST(LTrimTest, StringWithLeadingWhiteSpaceOnlyReturnsInputWithoutWhitespace) {
  constexpr TypeParam string{"     \f\n\r\t\vhello world"};
  constexpr TypeParam trimmed_string{"hello world"};
  STATIC_ASSERT_EQ(ltrim(string), trimmed_string);
}

/// @test `ltrim` returns the input if it only contains trailing whitespaces.
TYPED_TEST(LTrimTest, StringWithTrailingWhiteSpaceOnlyReturnsInput) {
  constexpr TypeParam string{"hello world   \f\n\r\t\v"};
  STATIC_ASSERT_EQ(ltrim(string), string);
}

/// @test `ltrim` trims leading whitespaces, and leaves trailing whitespaces.
TYPED_TEST(LTrimTest, StringWithLeadingAndTrailingWhiteSpaceReturnsInputWithoutLeadingWhitespace) {
  constexpr TypeParam string{"     \f\n\r\t\vhello world   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{"hello world   \f\n\r\t\v"};
  STATIC_ASSERT_EQ(ltrim(string), trimmed_string);
}

/// @test `ltrim` returns an empty string if the input is only whitespaces.
TYPED_TEST(LTrimTest, StringWithOnlyWhiteSpaceReturnsEmptyString) {
  constexpr TypeParam string{"     \f\n\r\t\v   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{""};
  STATIC_ASSERT_EQ(ltrim(string), trimmed_string);
}

/// @test `ltrim` returns an empty string if the input is empty.
TYPED_TEST(LTrimTest, EmptyStringReturnsEmptyString) {
  constexpr TypeParam string{""};
  constexpr TypeParam trimmed_string{""};
  STATIC_ASSERT_EQ(ltrim(string), trimmed_string);
}

/// @test `ltrim` returns the input if it only contains middle whitespaces.
TYPED_TEST(LTrimTest, StringWithMiddleWhiteSpaceOnlyReturnsInput) {
  constexpr TypeParam string{"hello\f\n\r\t\vworld"};
  STATIC_ASSERT_EQ(ltrim(string), string);
}

/// @test `ltrim` trims leading whitespaces, and leaves trailing and middle whitespaces.
TYPED_TEST(LTrimTest, StringWithMiddleWhiteSpaceAndNonMiddleWhitespaceReturnsLeadingTrimmed) {
  constexpr TypeParam string{" hello\f\n\r\t\vworld "};
  constexpr TypeParam trimmed_string{"hello\f\n\r\t\vworld "};
  STATIC_ASSERT_EQ(ltrim(string), trimmed_string);
}

template <typename StringType>
class RTrimTest : public TrimTestBase<StringType> {};
TYPED_TEST_SUITE(RTrimTest, string_types, );

/// @test `rtrim` returns the input if it contains no trailing whitespaces.
TYPED_TEST(RTrimTest, StringWithNoWhiteSpaceReturnsInput) {
  constexpr TypeParam string{"hello world"};
  STATIC_ASSERT_EQ(rtrim(string), string);
}

/// @test `rtrim` returns the input if it only contains leading whitespaces.
TYPED_TEST(RTrimTest, StringWithLeadingWhiteSpaceOnlyReturnsInput) {
  constexpr TypeParam string{"     \f\n\r\t\vhello world"};
  STATIC_ASSERT_EQ(rtrim(string), string);
}

/// @test `rtrim` trims trailing whitespaces.
TYPED_TEST(RTrimTest, StringWithTrailingWhiteSpaceOnlyReturnsInputWithoutWhitespace) {
  constexpr TypeParam string{"hello world   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{"hello world"};
  STATIC_ASSERT_EQ(rtrim(string), trimmed_string);
}

/// @test `rtrim` trims trailing whitespaces, and leaves leading whitespaces.
TYPED_TEST(RTrimTest, StringWithLeadingAndTrailingWhiteSpaceReturnsInputWithoutTrailingWhitespace) {
  constexpr TypeParam string{"     \f\n\r\t\vhello world   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{"     \f\n\r\t\vhello world"};
  STATIC_ASSERT_EQ(rtrim(string), trimmed_string);
}

/// @test `rtrim` returns an empty string if the input is only whitespaces.
TYPED_TEST(RTrimTest, StringWithOnlyWhiteSpaceReturnsEmptyString) {
  constexpr TypeParam string{"     \f\n\r\t\v   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{""};
  STATIC_ASSERT_EQ(rtrim(string), trimmed_string);
}

/// @test `rtrim` returns an empty string if the input is empty.
TYPED_TEST(RTrimTest, EmptyStringReturnsEmptyString) {
  constexpr TypeParam string{""};
  constexpr TypeParam trimmed_string{""};
  STATIC_ASSERT_EQ(rtrim(string), trimmed_string);
}

/// @test `rtrim` returns the input if it only contains middle whitespaces.
TYPED_TEST(RTrimTest, StringWithMiddleWhiteSpaceOnlyReturnsInput) {
  constexpr TypeParam string{"hello\f\n\r\t\vworld"};
  STATIC_ASSERT_EQ(rtrim(string), string);
}

/// @test `rtrim` trims trailing whitespaces, and leaves leading and middle whitespaces.
TYPED_TEST(RTrimTest, StringWithMiddleWhiteSpaceAndNonMiddleWhitespaceReturnsTrailingTrimmed) {
  constexpr TypeParam string{" hello\f\n\r\t\vworld "};
  constexpr TypeParam trimmed_string{" hello\f\n\r\t\vworld"};
  STATIC_ASSERT_EQ(rtrim(string), trimmed_string);
}

template <typename StringType>
class TrimTest : public TrimTestBase<StringType> {};
TYPED_TEST_SUITE(TrimTest, string_types, );

/// @test Trimming a string with no leading or trailing whitespace returns the input string
TYPED_TEST(TrimTest, StringWithNoWhiteSpaceReturnsInput) {
  constexpr TypeParam string{"hello world"};
  STATIC_ASSERT_EQ(trim(string), string);
}

/// @test `trim` trims leading whitespaces.
TYPED_TEST(TrimTest, StringWithLeadingWhiteSpaceOnlyReturnsInputWithoutWhitespace) {
  constexpr TypeParam string{"     \f\n\r\t\vhello world"};
  constexpr TypeParam trimmed_string{"hello world"};
  STATIC_ASSERT_EQ(trim(string), trimmed_string);
}

/// @test `trim` trims trailing whitespaces.
TYPED_TEST(TrimTest, StringWithTrailingWhiteSpaceOnlyReturnsInputWithoutWhitespace) {
  constexpr TypeParam string{"hello world   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{"hello world"};
  STATIC_ASSERT_EQ(trim(string), trimmed_string);
}

/// @test `trim` trims leading and trailing whitespaces.
TYPED_TEST(TrimTest, StringWithLeadingAndTrailingWhiteSpaceReturnsInputWithoutWhitespace) {
  constexpr TypeParam string{"     \f\n\r\t\vhello world   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{"hello world"};
  STATIC_ASSERT_EQ(trim(string), trimmed_string);
}

/// @test `trim` returns an empty string if the input is only whitespaces.
TYPED_TEST(TrimTest, StringWithOnlyWhiteSpaceReturnsEmptyString) {
  constexpr TypeParam string{"     \f\n\r\t\v   \f\n\r\t\v"};
  constexpr TypeParam trimmed_string{""};
  STATIC_ASSERT_EQ(trim(string), trimmed_string);
}

/// @test `trim` returns an empty string if the input is empty.
TYPED_TEST(TrimTest, EmptyStringReturnsEmptyString) {
  constexpr TypeParam string{""};
  constexpr TypeParam trimmed_string{""};
  STATIC_ASSERT_EQ(trim(string), trimmed_string);
}

/// @test `trim` returns the input if it only contains middle whitespaces.
TYPED_TEST(TrimTest, StringWithMiddleWhiteSpaceOnlyReturnsInput) {
  constexpr TypeParam string{"hello\f\n\r\t\vworld"};
  STATIC_ASSERT_EQ(trim(string), string);
}

/// @test `trim` trims leading and trailing whitespaces, and leaves middle whitespaces.
TYPED_TEST(TrimTest, StringWithMiddleWhiteSpaceAndNonMiddleWhitespaceReturnsNonMiddleTrimmed) {
  constexpr TypeParam string{" hello\f\n\r\t\vworld "};
  constexpr TypeParam trimmed_string{"hello\f\n\r\t\vworld"};
  STATIC_ASSERT_EQ(trim(string), trimmed_string);
}

}  // namespace
