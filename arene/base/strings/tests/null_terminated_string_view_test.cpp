// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/null_terminated_string_view.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <sstream>
#include <string>

namespace arene {
namespace base {
/// @brief prints the null terminated string view, gtest style.
///
/// We have to install a custom gtest printer here because otherwise in older GCCs gtest for whatever reason will not
/// see the operator<< overload, and will instead attempt to print using the container printer, which uses a
/// range-based-for-loop, which pre C++17 does not work if begin() and end() have mismatched types.
// NOLINTNEXTLINE(readability-identifier-naming) Gtest customization point, must be exactly this name.
void PrintTo(null_terminated_string_view const str, std::ostream* stream) { *stream << str; }

}  // namespace base
}  // namespace arene

#endif  // ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)

namespace {

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace arene::base::literals;

using arene::base::null_terminated_string_view;
using arene::base::string_view;

/// @test A default-constructed `null_terminated_string_view` refers to a zero-length string
TEST(NullTerminatedStringView, DefaultConstruction) {
  // Constructs to the empty string, not the null string.
  null_terminated_string_view const nt_sv;
  STATIC_ASSERT_EQ(null_terminated_string_view{}.length(), 0);
  ASSERT_NE(nt_sv.data(), nullptr);
  EXPECT_EQ(nt_sv.begin()[0], '\0');
  STATIC_ASSERT_TRUE(null_terminated_string_view{}.empty());
}

/// @test A `null_terminated_string_view` constructed from a string literal is not empty, has the length of the string
/// literal, and `c_str` and `data` point to the string characters.
TEST(NullTerminatedStringView, ConstructionFromStringLiteral) {
  null_terminated_string_view const nt_sv{"hello"};
  EXPECT_FALSE(nt_sv.empty());
  EXPECT_EQ(nt_sv.length(), 5);

  ASSERT_NE(nt_sv.data(), nullptr);
  EXPECT_EQ(nt_sv.data(), nt_sv.c_str());

  EXPECT_EQ(nt_sv.begin()[0], 'h');
  EXPECT_EQ(nt_sv.begin()[1], 'e');
  EXPECT_EQ(nt_sv.begin()[2], 'l');
  EXPECT_EQ(nt_sv.begin()[3], 'l');
  EXPECT_EQ(nt_sv.begin()[4], 'o');
  EXPECT_EQ(nt_sv.begin()[5], '\0');
}

/// @test `null_terminated_string_view` constructed via the `_ntsv` literal suffix refers to a string with the length
/// and contents of the string literal
TEST(NullTerminatedStringView, UserDefinedLiteral) {
  auto const nt_sv = "hello"_ntsv;
  ::testing::StaticAssertTypeEq<decltype(nt_sv), null_terminated_string_view const>();
  EXPECT_EQ(nt_sv.length(), 5);
  ASSERT_NE(nt_sv.data(), nullptr);

  EXPECT_EQ(nt_sv.begin()[0], 'h');
  EXPECT_EQ(nt_sv.begin()[1], 'e');
  EXPECT_EQ(nt_sv.begin()[2], 'l');
  EXPECT_EQ(nt_sv.begin()[3], 'l');
  EXPECT_EQ(nt_sv.begin()[4], 'o');
  EXPECT_EQ(nt_sv.begin()[5], '\0');
}

/// @test `null_terminated_string_view` constructed from a `std::string` refers to the character data in the
/// `std::string`, so `c_str` on the `std::string` and `data` on the `null_terminated_string_view` are equal
TEST(NullTerminatedStringView, ConstructionFromStdString) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    std::string const std_str;
    null_terminated_string_view const nt_sv = std_str;
    ASSERT_EQ(nt_sv.length(), std_str.size());
    ASSERT_EQ(nt_sv.size(), std_str.size());
    ASSERT_EQ(nt_sv.data(), std_str.c_str());
  }

  {
    std::string const std_str = "a non-empty string";
    null_terminated_string_view const nt_sv = std_str;
    ASSERT_EQ(nt_sv.length(), std_str.size());
    ASSERT_EQ(nt_sv.size(), std_str.size());
    ASSERT_EQ(nt_sv.data(), std_str.c_str());
  }
#else
  GTEST_SKIP() << "No std::string when using Arene Base stdlib implementation";
#endif
}

/// @test `null_terminated_string_view` has iterators that can be used to iterate forward and backwards through the
/// characters
TEST(NullTerminatedStringView, Iterators) {
  null_terminated_string_view const nt_sv{"0123456789"};

  STATIC_ASSERT_TRUE(std::is_default_constructible<null_terminated_string_view::iterator>::value);

  {
    auto itr = nt_sv.begin();
    auto itr_end = nt_sv.end();

    ASSERT_NE(itr, itr_end);
    EXPECT_EQ(itr, nt_sv.begin());

    EXPECT_EQ(*itr, '0');

    {
      decltype(auto) result = ++itr;
      EXPECT_EQ(&result, &itr);
    }
    EXPECT_EQ(*itr, '1');

    auto prev = itr++;
    EXPECT_EQ(*itr, '2');
    EXPECT_EQ(*prev, '1');

    prev = itr--;
    EXPECT_EQ(*itr, '1');
    EXPECT_EQ(*prev, '2');

    {
      decltype(auto) result = (itr += 3);
      EXPECT_EQ(&result, &itr);
    }
    EXPECT_EQ(*itr, '4');
    EXPECT_EQ(itr - nt_sv.begin(), 4);

    {
      decltype(auto) result = --itr;
      EXPECT_EQ(&result, &itr);
    }
    EXPECT_EQ(*itr, '3');

    {
      decltype(auto) result = (itr -= 2);
      EXPECT_EQ(&result, &itr);
    }
    EXPECT_EQ(*itr, '1');

    // Comparison with self
    EXPECT_TRUE(itr == itr);
    EXPECT_FALSE(itr != itr);
    EXPECT_FALSE(itr < itr);
    EXPECT_FALSE(itr > itr);
    EXPECT_TRUE(itr <= itr);
    EXPECT_TRUE(itr >= itr);

    // binary addition/subtraction
    auto itr2 = itr + 4;
    EXPECT_EQ(*itr2, '5');

    auto itr3 = itr2 - 2;
    EXPECT_EQ(*itr3, '3');

    // Comparison with other iterator.
    EXPECT_FALSE(itr == itr2);
    EXPECT_TRUE(itr != itr2);
    EXPECT_TRUE(itr < itr2);
    EXPECT_FALSE(itr2 < itr);
    EXPECT_FALSE(itr > itr2);
    EXPECT_TRUE(itr2 > itr);
    EXPECT_TRUE(itr <= itr2);
    EXPECT_FALSE(itr2 <= itr);
    EXPECT_FALSE(itr >= itr2);
    EXPECT_TRUE(itr2 >= itr);

    // Indexing operator
    EXPECT_EQ(itr3[0], '3');
    EXPECT_EQ(itr3[-1], '2');
    EXPECT_EQ(itr3[3], '6');
  }

  // Iterate to end()
  {
    std::size_t count = 0;
    auto itr = nt_sv.begin();
    while (itr != nt_sv.end()) {
      ++itr;
      ++count;
    }
    ASSERT_EQ(count, 10);
    ASSERT_EQ(itr - nt_sv.begin(), 10);

    // Ok to dereference iterator equal to end().
    ASSERT_EQ(itr, nt_sv.end());
    ASSERT_EQ(*itr, '\0');
  }
}

/// @test `null_terminated_string_view` is implicitly convertible to `string_view`
TEST(NullTerminatedStringView, ImplicitConversionToStringView) {
  null_terminated_string_view const nt_sv{"hello world"};
  string_view const str_view = nt_sv;
  ASSERT_EQ(str_view.length(), nt_sv.length());
  ASSERT_EQ(str_view.size(), nt_sv.size());
  ASSERT_EQ(str_view.data(), nt_sv.data());
}

/// @test Invoking `substr` with an offset returns a `null_terminated_string_view` referring to the tail of the string
/// starting at the specified offset.
TEST(NullTerminatedStringView, SubstringOffsetOnly) {
  null_terminated_string_view const nt_sv{"0123456789"};

  {
    auto const substr1 = nt_sv.substr(0);
    ::testing::StaticAssertTypeEq<decltype(nt_sv), null_terminated_string_view const>();
    ASSERT_EQ(substr1.length(), 10);
    ASSERT_EQ(substr1.data(), nt_sv.data());
  }

  {
    auto const substr1 = nt_sv.substr(4);
    ASSERT_EQ(substr1.length(), 6);
    ASSERT_EQ(substr1.data(), &nt_sv.begin()[4]);
  }

  {
    auto const substr1 = nt_sv.substr(10);
    ASSERT_EQ(substr1.length(), 0);
    ASSERT_EQ(substr1.data(), &nt_sv.begin()[10]);
  }

  {
    auto const substr1 = nt_sv.substr(11);
    ASSERT_EQ(substr1.length(), 0);
    ASSERT_EQ(substr1.data(), &nt_sv.begin()[10]);
  }

  {
    auto const substr1 = nt_sv.substr(20);
    ASSERT_EQ(substr1.length(), 0);
    ASSERT_EQ(substr1.data(), &nt_sv.begin()[10]);
  }
}

/// @test Invoking `substr` with an offset and a length returns a `string_view` referring to the portion of the string
/// starting at the specified offset up to the specified length
TEST(NullTerminatedStringView, SubstringOffsetAndMaxLength) {
  constexpr auto* original_string = "0123456789";
  constexpr null_terminated_string_view nt_sv{original_string};
  constexpr string_view str_view{original_string};
  constexpr std::size_t zero_index = 0;
  constexpr std::size_t offset_index = 3;
  constexpr std::size_t last_index = nt_sv.length();
  constexpr std::size_t past_the_end_index = last_index + 2;
  constexpr std::size_t zero_length = 0;
  constexpr std::size_t non_zero_length = 3;
  constexpr std::size_t max_length = last_index;

  STATIC_ASSERT_EQ(nt_sv.substr(zero_index, zero_length), str_view.substr(zero_index, zero_length));
  STATIC_ASSERT_EQ(nt_sv.substr(zero_index, non_zero_length), str_view.substr(zero_index, non_zero_length));
  STATIC_ASSERT_EQ(nt_sv.substr(zero_index, max_length), str_view.substr(zero_index, max_length));
  STATIC_ASSERT_EQ(nt_sv.substr(zero_index, max_length + 1), str_view.substr(zero_index, max_length + 1));
  STATIC_ASSERT_EQ(nt_sv.substr(offset_index, zero_length), str_view.substr(offset_index, zero_length));
  STATIC_ASSERT_EQ(nt_sv.substr(offset_index, non_zero_length), str_view.substr(offset_index, non_zero_length));
  STATIC_ASSERT_EQ(
      nt_sv.substr(offset_index, max_length - offset_index),
      str_view.substr(offset_index, max_length - offset_index)
  );
  STATIC_ASSERT_EQ(
      nt_sv.substr(offset_index, max_length - offset_index + 1),
      str_view.substr(offset_index, max_length - offset_index + 1)
  );
  STATIC_ASSERT_EQ(nt_sv.substr(last_index, zero_length), str_view.substr(last_index, zero_length));
  STATIC_ASSERT_EQ(nt_sv.substr(last_index, non_zero_length), str_view.substr(last_index, non_zero_length));
  STATIC_ASSERT_EQ(nt_sv.substr(past_the_end_index, zero_length), str_view.substr(last_index, zero_length));
  STATIC_ASSERT_EQ(nt_sv.substr(past_the_end_index, non_zero_length), str_view.substr(last_index, non_zero_length));
}

/// @test Invoking `compare` with another `null_terminated_string_view` returns values consistent with lexicographic
/// ordering and `std::string` ordering of UTF-8 strings.
TEST(NullTerminatedStringView, CompareWithNullTerminated) {
  constexpr null_terminated_string_view k_empty;
  constexpr null_terminated_string_view k_hello{"hello"};
  constexpr null_terminated_string_view k_hello_world{"hello world"};
  constexpr null_terminated_string_view k_hello_high_chars{u8"hello\u2009world"};
  constexpr null_terminated_string_view k_high_chars{u8"\u00A5"};

  STATIC_ASSERT_EQ(k_empty.compare(k_empty), 0);
  STATIC_ASSERT_LT(k_empty.compare(k_hello), 0);
  STATIC_ASSERT_LT(k_empty.compare(k_high_chars), 0);
  STATIC_ASSERT_GT(k_hello.compare(k_empty), 0);

  STATIC_ASSERT_GT(k_hello.compare(k_empty), 0);
  STATIC_ASSERT_EQ(k_hello.compare(k_hello), 0);
  STATIC_ASSERT_LT(k_hello.compare(k_hello_world), 0);
  STATIC_ASSERT_LT(k_hello.compare(k_hello_high_chars), 0);
  STATIC_ASSERT_LT(k_hello.compare(k_high_chars), 0);

  STATIC_ASSERT_GT(k_hello_world.compare(k_empty), 0);
  STATIC_ASSERT_GT(k_hello_world.compare(k_hello), 0);
  STATIC_ASSERT_EQ(k_hello_world.compare(k_hello_world), 0);
  STATIC_ASSERT_LT(k_hello_world.compare(k_hello_high_chars), 0);
  STATIC_ASSERT_LT(k_hello_world.compare(k_high_chars), 0);

  STATIC_ASSERT_GT(k_hello_high_chars.compare(k_empty), 0);
  STATIC_ASSERT_GT(k_hello_high_chars.compare(k_hello), 0);
  STATIC_ASSERT_GT(k_hello_high_chars.compare(k_hello_world), 0);
  STATIC_ASSERT_EQ(k_hello_high_chars.compare(k_hello_high_chars), 0);
  STATIC_ASSERT_LT(k_hello_high_chars.compare(k_high_chars), 0);

  STATIC_ASSERT_GT(k_high_chars.compare(k_empty), 0);
  STATIC_ASSERT_GT(k_high_chars.compare(k_hello), 0);
  STATIC_ASSERT_GT(k_high_chars.compare(k_hello_world), 0);
  STATIC_ASSERT_GT(k_high_chars.compare(k_hello_high_chars), 0);
  STATIC_ASSERT_EQ(k_high_chars.compare(k_high_chars), 0);
}

/// @test Invoking `compare` with a `string_view` returns values consistent with lexicographic ordering and
/// `std::string` ordering of UTF-8 strings.
TEST(NullTerminatedStringView, CompareWithStringView) {
  constexpr null_terminated_string_view k_empty;
  constexpr null_terminated_string_view k_hello{"hello"};
  constexpr null_terminated_string_view k_hello_world{"hello world"};
  constexpr null_terminated_string_view k_hello_high_chars{u8"hello\u2009world"};
  constexpr null_terminated_string_view k_high_chars{u8"\u00A5"};

  STATIC_ASSERT_EQ(k_empty.compare(static_cast<string_view>(k_empty)), 0);
  STATIC_ASSERT_LT(k_empty.compare(static_cast<string_view>(k_hello)), 0);
  STATIC_ASSERT_LT(k_empty.compare(static_cast<string_view>(k_high_chars)), 0);
  STATIC_ASSERT_GT(k_hello.compare(static_cast<string_view>(k_empty)), 0);

  STATIC_ASSERT_GT(k_hello.compare(static_cast<string_view>(k_empty)), 0);
  STATIC_ASSERT_EQ(k_hello.compare(static_cast<string_view>(k_hello)), 0);
  STATIC_ASSERT_LT(k_hello.compare(static_cast<string_view>(k_hello_world)), 0);
  STATIC_ASSERT_LT(k_hello.compare(static_cast<string_view>(k_hello_high_chars)), 0);
  STATIC_ASSERT_LT(k_hello.compare(static_cast<string_view>(k_high_chars)), 0);

  STATIC_ASSERT_GT(k_hello_world.compare(static_cast<string_view>(k_empty)), 0);
  STATIC_ASSERT_GT(k_hello_world.compare(static_cast<string_view>(k_hello)), 0);
  STATIC_ASSERT_EQ(k_hello_world.compare(static_cast<string_view>(k_hello_world)), 0);
  STATIC_ASSERT_LT(k_hello_world.compare(static_cast<string_view>(k_hello_high_chars)), 0);
  STATIC_ASSERT_LT(k_hello_world.compare(static_cast<string_view>(k_high_chars)), 0);

  STATIC_ASSERT_GT(k_hello_high_chars.compare(static_cast<string_view>(k_empty)), 0);
  STATIC_ASSERT_GT(k_hello_high_chars.compare(static_cast<string_view>(k_hello)), 0);
  STATIC_ASSERT_GT(k_hello_high_chars.compare(static_cast<string_view>(k_hello_world)), 0);
  STATIC_ASSERT_EQ(k_hello_high_chars.compare(static_cast<string_view>(k_hello_high_chars)), 0);
  STATIC_ASSERT_LT(k_hello_high_chars.compare(static_cast<string_view>(k_high_chars)), 0);

  STATIC_ASSERT_GT(k_high_chars.compare(static_cast<string_view>(k_empty)), 0);
  STATIC_ASSERT_GT(k_high_chars.compare(static_cast<string_view>(k_hello)), 0);
  STATIC_ASSERT_GT(k_high_chars.compare(static_cast<string_view>(k_hello_world)), 0);
  STATIC_ASSERT_GT(k_high_chars.compare(static_cast<string_view>(k_hello_high_chars)), 0);
  STATIC_ASSERT_EQ(k_high_chars.compare(static_cast<string_view>(k_high_chars)), 0);
}

/// @test Invoking `compare` with a `std::string` returns values consistent with lexicographic ordering and
/// `std::string` ordering of UTF-8 strings.
TEST(NullTerminatedStringView, CompareWithString) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  null_terminated_string_view const k_empty;
  null_terminated_string_view const k_hello{"hello"};
  null_terminated_string_view const k_hello_world{"hello world"};
  null_terminated_string_view const k_hello_high_chars{u8"hello\u2009world"};
  null_terminated_string_view const k_high_chars{u8"\u00A5"};

  std::string const k_empty2;
  std::string const k_hello2{"hello"};
  std::string const k_hello_world2{"hello world"};
  std::string const k_hello_high_chars2{u8"hello\u2009world"};
  std::string const k_high_chars2{u8"\u00A5"};

  EXPECT_EQ(k_empty.compare(k_empty2), 0);
  EXPECT_LT(k_empty.compare(k_hello2), 0);
  EXPECT_LT(k_empty.compare(k_high_chars2), 0);

  EXPECT_GT(k_hello.compare(k_empty2), 0);
  EXPECT_EQ(k_hello.compare(k_hello2), 0);
  EXPECT_LT(k_hello.compare(k_hello_world2), 0);
  EXPECT_LT(k_hello.compare(k_hello_high_chars2), 0);
  EXPECT_LT(k_hello.compare(k_high_chars2), 0);

  EXPECT_GT(k_hello_world.compare(k_empty2), 0);
  EXPECT_GT(k_hello_world.compare(k_hello2), 0);
  EXPECT_EQ(k_hello_world.compare(k_hello_world2), 0);
  EXPECT_LT(k_hello_world.compare(k_hello_high_chars2), 0);
  EXPECT_LT(k_hello_world.compare(k_high_chars2), 0);

  EXPECT_GT(k_hello_high_chars.compare(k_empty2), 0);
  EXPECT_GT(k_hello_high_chars.compare(k_hello2), 0);
  EXPECT_GT(k_hello_high_chars.compare(k_hello_world2), 0);
  EXPECT_EQ(k_hello_high_chars.compare(k_hello_high_chars2), 0);
  EXPECT_LT(k_hello_high_chars.compare(k_high_chars2), 0);

  EXPECT_GT(k_high_chars.compare(k_empty2), 0);
  EXPECT_GT(k_high_chars.compare(k_hello2), 0);
  EXPECT_GT(k_high_chars.compare(k_hello_world2), 0);
  EXPECT_GT(k_high_chars.compare(k_hello_high_chars2), 0);
  EXPECT_EQ(k_high_chars.compare(k_high_chars2), 0);
#else
  GTEST_SKIP() << "No std::string when using Arene Base stdlib implementation";
#endif
}

/// @test The comparison operators on `null_terminated_string_view` and another `null_terminated_string_view` or
/// `string_view` return results consistent with std::string lexicographic ordering
TEST(NullTerminatedStringView, ComparisonOperators) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto check_equal = [](auto const& lhs, auto const& rhs, bool result) {
    EXPECT_EQ((lhs == rhs), result);
    EXPECT_EQ((rhs == lhs), result);
    EXPECT_EQ((lhs != rhs), !result);
    EXPECT_EQ((rhs != lhs), !result);

    if (result) {
      EXPECT_FALSE(lhs < rhs);
      EXPECT_TRUE(lhs <= rhs);
      EXPECT_TRUE(lhs >= rhs);
      EXPECT_FALSE(lhs > rhs);

      EXPECT_FALSE(rhs < lhs);
      EXPECT_TRUE(rhs <= lhs);
      EXPECT_TRUE(rhs >= lhs);
      EXPECT_FALSE(rhs > lhs);
    }
  };

  auto check_less = [&](auto const& lhs, auto const& rhs) {
    check_equal(lhs, rhs, false);

    EXPECT_TRUE(lhs < rhs);
    EXPECT_TRUE(lhs <= rhs);
    EXPECT_FALSE(lhs >= rhs);
    EXPECT_FALSE(lhs > rhs);

    EXPECT_FALSE(rhs < lhs);
    EXPECT_FALSE(rhs <= lhs);
    EXPECT_TRUE(rhs >= lhs);
    EXPECT_TRUE(rhs > lhs);
  };

  auto check_strings_equal = [&](null_terminated_string_view const& lhs, null_terminated_string_view const& rhs) {
    check_equal(lhs, rhs, true);

    std::string const k_a_string{lhs.data(), lhs.length()};
    std::string const k_b_string{rhs.data(), rhs.length()};

    check_equal(lhs, k_b_string, true);
    check_equal(k_a_string, rhs, true);

    string_view a_view = lhs;
    string_view b_view = rhs;

    check_equal(lhs, b_view, true);
    check_equal(a_view, rhs, true);

    a_view = k_a_string;
    b_view = k_b_string;

    check_equal(lhs, b_view, true);
    check_equal(a_view, rhs, true);
  };

  auto check_strings_less = [&](null_terminated_string_view const& lhs, null_terminated_string_view const& rhs) {
    check_less(lhs, rhs);

    std::string const k_a_string{lhs.data(), lhs.length()};
    std::string const k_b_string{rhs.data(), rhs.length()};

    check_less(lhs, k_b_string);
    check_less(k_a_string, rhs);

    string_view a_view = lhs;
    string_view b_view = rhs;

    check_less(lhs, b_view);
    check_less(a_view, rhs);

    a_view = k_a_string;
    b_view = k_b_string;

    check_less(lhs, b_view);
    check_less(a_view, rhs);
  };

  constexpr null_terminated_string_view k_empty;
  constexpr null_terminated_string_view k_hello{"hello"};
  constexpr null_terminated_string_view k_hello_world{"hello world"};
  constexpr null_terminated_string_view k_hello_high_chars{u8"hello\u2009world"};
  constexpr null_terminated_string_view k_high_chars{u8"\u00A5"};

  check_strings_equal(k_empty, k_empty);
  check_strings_less(k_empty, k_hello);
  check_strings_less(k_empty, k_high_chars);

  // NOLINTNEXTLINE(misc-redundant-expression)
  STATIC_ASSERT_EQ(k_empty, k_empty);
  STATIC_ASSERT_LT(k_empty, k_hello);
  STATIC_ASSERT_LT(k_empty, k_high_chars);
  STATIC_ASSERT_EQ(static_cast<string_view>(k_empty), k_empty);
  STATIC_ASSERT_LT(static_cast<string_view>(k_empty), k_hello);
  STATIC_ASSERT_LT(static_cast<string_view>(k_empty), k_high_chars);
  STATIC_ASSERT_GT(k_hello, static_cast<string_view>(k_empty));
  STATIC_ASSERT_GT(k_high_chars, static_cast<string_view>(k_empty));
  STATIC_ASSERT_EQ(k_empty, static_cast<string_view>(k_empty));
  STATIC_ASSERT_LT(k_empty, static_cast<string_view>(k_hello));
  STATIC_ASSERT_LT(k_empty, static_cast<string_view>(k_high_chars));
  STATIC_ASSERT_GT(static_cast<string_view>(k_hello), k_empty);
  STATIC_ASSERT_GT(static_cast<string_view>(k_high_chars), k_empty);

  check_strings_equal(k_hello, k_hello);
  check_strings_less(k_hello, k_hello_world);
  check_strings_less(k_hello, k_hello_high_chars);
  check_strings_less(k_hello, k_high_chars);

  check_strings_equal(k_hello_world, k_hello_world);
  check_strings_less(k_hello_world, k_hello_high_chars);
  check_strings_less(k_hello_world, k_high_chars);

  check_strings_equal(k_hello_high_chars, k_hello_high_chars);
  check_strings_less(k_hello_high_chars, k_high_chars);

  check_strings_equal(k_high_chars, k_high_chars);
#else
  GTEST_SKIP() << "No std::string when using Arene Base stdlib implementation";
#endif
}

/// @test `null_terminated_string_view` can be compared for equality or inequality with a pointer to a NUL-terminated
/// string
TEST(NullTerminatedStringView, MixedComparisons) {
  constexpr null_terminated_string_view nt_sv{"hello"};

  STATIC_ASSERT_EQ(nt_sv, "hello");
  STATIC_ASSERT_NE(nt_sv, "goodbye");
  STATIC_ASSERT_EQ("hello", nt_sv);
  STATIC_ASSERT_NE("goodbye", nt_sv);
}

/// @test A `null_terminated_string_view` cannot be constructed from an initializer list
TEST(NullTerminatedStringView, NotConstructibleFromInitializerList) {
  // This test ensures that while we can construct from a std::string, we
  // shouldn't allow constructing from things that implicitly convert to a
  // std::string as this can lead to the resulting string view containing
  // a reference to a temporary.
  STATIC_ASSERT_FALSE(std::is_constructible<null_terminated_string_view, std::initializer_list<char>>::value);
}

/// This function checks that all operations are valid to use in a
/// constexpr context.
constexpr auto constexpr_iterator_test(null_terminated_string_view nt_sv) -> bool {
  auto itr = nt_sv.begin();
  ++itr;
  auto itr1 = itr++;
  --itr;
  auto itr2 = itr--;
  if (itr1 != itr2) {
    return false;
  }
  itr += 3;
  itr -= 3;
  char const a_char = itr[0];
  if (a_char != 'h') {
    return false;
  }
  if (itr1 < itr) {
    return false;
  }
  if (itr1 <= itr) {
    return false;
  }
  if (itr > itr1) {
    return false;
  }
  if (itr >= itr1) {
    return false;
  }
  auto itr3 = itr + 5;
  auto itr4 = itr3 - 4;
  if (itr4 != itr1) {
    return false;
  }
  if (nt_sv.empty()) {
    return false;
  }
  if (nt_sv.substr(0) != nt_sv) {
    return false;
  }
  if (nt_sv.substr(0, nt_sv.length()) != nt_sv) {
    return false;
  }
  if (nt_sv.compare(nt_sv) != 0) {
    return false;
  }
  if (nt_sv.compare(static_cast<string_view>(nt_sv)) != 0) {
    return false;
  }
  if (nt_sv.substr(1) == nt_sv) {
    return false;
  }
  if (nt_sv.substr(1, nt_sv.length()) == nt_sv) {
    return false;
  }
  return true;
}

/// @test `null_terminated_string_view` can be used in a `constexpr` context
TEST(NullTerminatedStringView, ConstexprUsage) {
  constexpr auto k_string = "hello world"_ntsv;

  STATIC_ASSERT_EQ(k_string.length(), 11);
  STATIC_ASSERT_EQ(k_string.data(), k_string.c_str());
  STATIC_ASSERT_NE(k_string.begin(), k_string.end());
  STATIC_ASSERT_EQ(k_string.begin(), k_string.begin());

  STATIC_ASSERT_TRUE(constexpr_iterator_test(k_string));
}

/// @test Writing a `null_terminated_string_view` to a `std::ostream` copies the characters to the stream.
TEST(NullTerminatedStringViewStreamOperator, IsEquivalentToPrintingUnderlyingString) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  std::string const original{"hello"};
  null_terminated_string_view const nt_sv{original};
  std::stringstream stream;
  stream << nt_sv;
  EXPECT_EQ(stream.str(), original);
#else
  GTEST_SKIP() << "No stream operator when using Arene Base stdlib implementation";
#endif
}

}  // namespace
