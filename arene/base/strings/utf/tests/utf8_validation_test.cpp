// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/utf/utf8_validation.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/strings/string_view.hpp"

namespace {

using ::arene::base::array;
using ::arene::base::is_valid_utf8;

/// @test An empty string is valid UTF-8
TEST(Utf8, EmptyStringIsValidUtf8) {
  ASSERT_TRUE(is_valid_utf8(""));
  static_assert(is_valid_utf8(""), "Empty string is valid");
}

/// @test UTF-8 marker characters on their own are not valid
TEST(Utf8, SingleMarkerCharactersNotValid) {
  constexpr array<char, 1> arr1{static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "Invalid char");

  constexpr array<char, 1> arr2{static_cast<char>(0XC0)};
  ASSERT_FALSE(is_valid_utf8({arr2.data(), arr2.size()}));
  static_assert(!is_valid_utf8({arr2.data(), arr2.size()}), "Invalid char");

  constexpr array<char, 1> arr3{static_cast<char>(0XE0)};
  ASSERT_FALSE(is_valid_utf8({arr3.data(), arr3.size()}));
  static_assert(!is_valid_utf8({arr3.data(), arr3.size()}), "Invalid char");

  constexpr array<char, 1> arr4{static_cast<char>(0XF0)};
  ASSERT_FALSE(is_valid_utf8({arr4.data(), arr4.size()}));
  static_assert(!is_valid_utf8({arr4.data(), arr4.size()}), "Invalid char");

  constexpr array<char, 1> arr5{static_cast<char>(0XF8)};
  ASSERT_FALSE(is_valid_utf8({arr5.data(), arr5.size()}));
  static_assert(!is_valid_utf8({arr5.data(), arr5.size()}), "Invalid char");
}

/// @test A single ASCII character is valid UTF-8
TEST(Utf8, SingleAsciiCharIsValid) {
  constexpr array<char, 1> arr1{0X30};
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");

  constexpr array<char, 1> arr2{0X7F};
  ASSERT_TRUE(is_valid_utf8({arr2.data(), arr2.size()}));
  static_assert(is_valid_utf8({arr2.data(), arr2.size()}), "valid char");
}

/// @test A lead character indicating one follower, and a single follower character is valid.
TEST(Utf8, SingleLeadFollowedByContinuationIsValid) {
  constexpr array<char, 2> arr1{static_cast<char>(0XC2), static_cast<char>(0X80)};
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");
}

/// @test A lead character indicating one follower, followed by an ASCII character is invalid
TEST(Utf8, SingleLeadFollowedByAsciiCharIsInvalid) {
  constexpr array<char, 2> arr1{static_cast<char>(0XC2), static_cast<char>(0X30)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "Invalid char");
}

/// @test Two lead characters in a row is invalid
TEST(Utf8, TwoLeadsInARowIsInvalid) {
  constexpr array<char, 2> arr1{static_cast<char>(0XC2), static_cast<char>(0XC2)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "Invalid char");
}

/// @test Splitting a 7-bit ASCII character into lead+follower is invalid
TEST(Utf8, Overlong7bitCharacterInvalid) {
  constexpr array<char, 2> arr1{static_cast<char>(0XC1), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test A lead character indicating two followers, and a single follower character is invalid.
TEST(Utf8, ThreeByteLeadWithOneFollowonIsInvalid) {
  constexpr array<char, 2> arr1{static_cast<char>(0XEF), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "Invalid char");
}

/// @test A lead character indicating two followers, and two follower characters is valid.
TEST(Utf8, ThreeByteLeadWithTwoFollowonIsValid) {
  constexpr array<char, 3> arr1{static_cast<char>(0XEF), static_cast<char>(0X80), static_cast<char>(0X80)};
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");
}

/// @test Splitting an 11-bit code point into lead+2 followers is invalid
TEST(Utf8, Overlong11bitCharacterAs3BytesInvalid) {
  constexpr array<char, 3> arr1{static_cast<char>(0XE0), static_cast<char>(0X9F), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test The minimum code point that requires 3 bytes is valid as 3 bytes
TEST(Utf8, Min3ByteNotOverLongIsValid) {
  constexpr array<char, 3> arr1{static_cast<char>(0XE0), static_cast<char>(0XA0), static_cast<char>(0X80)};
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test A lead character indicating two followers, followed by an ASCII character and a follower character is invalid.
TEST(Utf8, ThreeByteLeadWithLowSecondIsInvalid) {
  constexpr array<char, 3> arr1{static_cast<char>(0XEF), static_cast<char>(0X30), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test A lead character indicating two followers, followed by another lead character is invalid.
TEST(Utf8, ThreeByteLeadWithLeadSecondIsInvalid) {
  constexpr array<char, 3> arr1{static_cast<char>(0XEF), static_cast<char>(0XC2), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test A UTF-16 surrogate pair encoded as UTF-8 is invalid
TEST(Utf8, ThreeByteEncodingOfSurrogatePairCharIsInvalid) {
  // ED A0 80 => codepoint 0xD800, which is a UTF16 surrogate pair
  constexpr array<char, 3> arr1{static_cast<char>(0XED), static_cast<char>(0XA0), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");

  // ED BF BF => codepoint 0xDFFF, which is a UTF16 surrogate pair
  constexpr array<char, 3> arr2{static_cast<char>(0XED), static_cast<char>(0XBF), static_cast<char>(0XBF)};
  ASSERT_FALSE(is_valid_utf8({arr2.data(), arr2.size()}));
  static_assert(!is_valid_utf8({arr2.data(), arr2.size()}), "invalid char");
}

/// @test The code points that are immediately adjacent to the surrogate pair range can be encoded as 3-byte UTF-8 and
/// are valid
TEST(Utf8, ThreeByteEncodingOfCharactersEitherSideOfSurrogatesIsValid) {
  // ED 9F BF => codepoint 0xD7FF, which is not a UTF16 surrogate pair
  constexpr array<char, 3> arr1{static_cast<char>(0XED), static_cast<char>(0X9F), static_cast<char>(0XBF)};
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");

  // EE 80 80 => codepoint 0xE000, which is not a UTF16 surrogate pair
  constexpr array<char, 3> arr2{static_cast<char>(0XEE), static_cast<char>(0X80), static_cast<char>(0X80)};
  ASSERT_TRUE(is_valid_utf8({arr2.data(), arr2.size()}));
  static_assert(is_valid_utf8({arr2.data(), arr2.size()}), "valid char");
}

/// @test A lead character indicating two followers, followed by another lead character is invalid.
TEST(Utf8, ThreeByteEncodingWithSecondAsLeadInIsInvalid) {
  constexpr array<char, 3> arr1{static_cast<char>(0XED), static_cast<char>(0XC2), static_cast<char>(0X80)};
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test A 4-byte encoding can be valid UTF-8
TEST(Utf8, FourByteEncodingCanBeValid) {
  constexpr array<char, 4> arr1{
      static_cast<char>(0XF1),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");
}

/// @test A lead character indicating three followers, that is not followed by 3 "follower" characters is invalid UTF-8
TEST(Utf8, FourByteEncodingWithBadContinuationsInvalid) {
  constexpr array<char, 4> arr1{
      static_cast<char>(0XF1),
      static_cast<char>(0X30),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");

  constexpr array<char, 4> arr2{
      static_cast<char>(0XF1),
      static_cast<char>(0X80),
      static_cast<char>(0X30),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr2.data(), arr2.size()}));
  static_assert(!is_valid_utf8({arr2.data(), arr2.size()}), "invalid char");

  constexpr array<char, 4> arr3{
      static_cast<char>(0XF1),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0X30)
  };
  ASSERT_FALSE(is_valid_utf8({arr3.data(), arr3.size()}));
  static_assert(!is_valid_utf8({arr3.data(), arr3.size()}), "invalid char");

  constexpr array<char, 4> arr4{
      static_cast<char>(0XF1),
      static_cast<char>(0XC2),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr4.data(), arr4.size()}));
  static_assert(!is_valid_utf8({arr4.data(), arr4.size()}), "invalid char");

  constexpr array<char, 4> arr5{
      static_cast<char>(0XF1),
      static_cast<char>(0X80),
      static_cast<char>(0XC2),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr5.data(), arr5.size()}));
  static_assert(!is_valid_utf8({arr5.data(), arr5.size()}), "invalid char");

  constexpr array<char, 4> arr6{
      static_cast<char>(0XF1),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0XC2)
  };
  ASSERT_FALSE(is_valid_utf8({arr6.data(), arr6.size()}));
  static_assert(!is_valid_utf8({arr6.data(), arr6.size()}), "invalid char");
}

/// @test The minimum code point that requires a 4-byte encoding is valid UTF-8
TEST(Utf8, FourByteEncodingWithMinValueIsValid) {
  constexpr array<char, 4> arr1{
      static_cast<char>(0XF0),
      static_cast<char>(0X90),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");
}

/// @test Encoding a codepoint that could be done as 3 bytes in 4 bytes is invalid
TEST(Utf8, FourByteEncodingWithOverlong16bitValueInvalid) {
  constexpr array<char, 4> arr1{
      static_cast<char>(0XF0),
      static_cast<char>(0X8F),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
}

/// @test Encoding a codepoint beyond the end of the valid Unicode codepoint range is invalid
TEST(Utf8, FourByteEncodingWithOverflowValueInvalid) {
  constexpr array<char, 4> arr1{
      static_cast<char>(0XF4),
      static_cast<char>(0X90),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(!is_valid_utf8({arr1.data(), arr1.size()}), "invalid char");
  constexpr array<char, 4> arr2{
      static_cast<char>(0XF5),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr2.data(), arr2.size()}));
  static_assert(!is_valid_utf8({arr2.data(), arr2.size()}), "invalid char");
  constexpr array<char, 4> arr3{
      static_cast<char>(0XF6),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr3.data(), arr3.size()}));
  static_assert(!is_valid_utf8({arr3.data(), arr3.size()}), "invalid char");
  constexpr array<char, 4> arr4{
      static_cast<char>(0XF7),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr4.data(), arr4.size()}));
  static_assert(!is_valid_utf8({arr4.data(), arr4.size()}), "invalid char");
  constexpr array<char, 4> arr5{
      static_cast<char>(0XFF),
      static_cast<char>(0X80),
      static_cast<char>(0X80),
      static_cast<char>(0X80)
  };
  ASSERT_FALSE(is_valid_utf8({arr5.data(), arr5.size()}));
  static_assert(!is_valid_utf8({arr5.data(), arr5.size()}), "invalid char");
}

/// @test The maximum Unicode codepoint encoded as UTF-8 is valid
TEST(Utf8, FourByteEncodingWithMaxValueIsValid) {
  constexpr array<char, 4> arr1{
      static_cast<char>(0XF4),
      static_cast<char>(0X8F),
      static_cast<char>(0XBF),
      static_cast<char>(0XBF)
  };
  ASSERT_TRUE(is_valid_utf8({arr1.data(), arr1.size()}));
  static_assert(is_valid_utf8({arr1.data(), arr1.size()}), "valid char");
}

/// @test sequences of UTF-8 characters are valid, including characters that consist of multiple "combining" code points
TEST(Utf8, ValidUtf8Sequences) {
  static_assert(is_valid_utf8(u8""), "Valid sequence");
  static_assert(is_valid_utf8(u8"abc"), "Valid sequence");
  static_assert(is_valid_utf8(u8"%*%&*^NJ*(jnnkjnkuh"), "Valid sequence");
  static_assert(is_valid_utf8(u8"名古屋市"), "Valid sequence");
  static_assert(is_valid_utf8(u8"abc名古屋市def"), "Valid sequence");
  static_assert(is_valid_utf8(u8"大阪市"), "Valid sequence");
  static_assert(is_valid_utf8(u8"東京"), "Valid sequence");
  static_assert(is_valid_utf8(u8"ア"), "Valid sequence");
  static_assert(is_valid_utf8(u8"🇬🇧"), "Valid sequence");
  static_assert(is_valid_utf8(u8"🇯🇵"), "Valid sequence");
  static_assert(is_valid_utf8(u8"🇺🇸"), "Valid sequence");
  static_assert(is_valid_utf8(u8"😂🤣☠️😻"), "Valid sequence");
}

/// @test Checking for valid UTF-8 is `noexcept`
TEST(Utf8, Utf8ValidationIsNoexcept) { static_assert(noexcept(is_valid_utf8(u8"名古屋市")), "must not throw"); }

}  // namespace
