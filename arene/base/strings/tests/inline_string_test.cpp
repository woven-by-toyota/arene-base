// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/inline_string.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/fill.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/reverse.hpp"
#include "arene/base/strings/algorithm/length.hpp"
#include "arene/base/strings/detail/string_length.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

using ::arene::base::inline_string;
using ::arene::base::literals::operator""_asv;
using ::arene::base::null_terminated_string_view;
using ::arene::base::nullopt;
using ::arene::base::string_view;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

template <std::size_t Size>
constexpr auto make_character_buffer(char fill_with) noexcept -> arene::base::array<char, Size + 1UL> {
  static_assert(Size + 1UL > 0UL, "Size must not wrap");
  arene::base::array<char, Size + 1UL> buffer{};
  arene::base::fill(buffer.begin(), buffer.end(), fill_with);
  buffer[Size] = '\0';
  return buffer;
}

constexpr arene::base::array<char, default_size + 1UL> exact_length_buffer = make_character_buffer<default_size>('a');
constexpr string_view exact_length_view{exact_length_buffer.data(), default_size};

constexpr arene::base::array<char, default_size + 2UL> too_long_buffer = make_character_buffer<default_size + 1UL>('a');
constexpr string_view too_long_view{too_long_buffer.data(), default_size + 1UL};

/// @test A default-constructed `inline_string` is empty, with a size of 0, and `c_str` returns a pointer to a
/// zero-length NUL-terminated string
TEST(InlineStringCtors, DefaultConstructedStringsAreBlank) {
  constexpr inline_string<default_size> default_ctord_string;

  ASSERT_TRUE(default_ctord_string.empty());
  ASSERT_EQ(default_ctord_string.size(), 0);
  ASSERT_NE(default_ctord_string.c_str(), nullptr);
  ASSERT_EQ(*default_ctord_string.c_str(), 0);

  ASSERT_TRUE(std::is_trivially_destructible<inline_string<default_size>>::value);
}

/// @test If an `inline_string` is constructed from a string literal, then the length of the string is the same as that
/// of the string literal, and `c_str` returns a pointer to a NUL-terminated string with the same contents as the string
/// literal.
TEST(InlineStringCtors, IfInitializedFromStringLiteralContentsAreTheSame) {
  inline_string<default_size> const literal_initialized("hello");

  ASSERT_TRUE(!literal_initialized.empty());
  ASSERT_EQ(literal_initialized.size(), arene::base::string_length("hello"));
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wstring-compare", "We're deliberately comparing the adresses of these strings");
  ARENE_IGNORE_GCC("-Waddress", "We're deliberately comparing the adresses of these strings");
  ASSERT_NE(literal_initialized.c_str(), nullptr);
  ASSERT_NE(literal_initialized.c_str(), "hello");
  ARENE_IGNORE_END();
  ASSERT_EQ(*literal_initialized.c_str(), 'h');
  ASSERT_STREQ(literal_initialized.c_str(), "hello");
}

/// @test If an `inline_string` is constructed from a given length and a character, the initialized string is filled
/// with the given character with the given length.
TEST(InlineStringCtors, IfInitializedFromLengthAndCharacterAsPrefilledString) {
  static constexpr std::size_t len{6U};
  static constexpr char chr{'b'};
  inline_string<default_size> const string_initialized{len, chr};

  ASSERT_FALSE(string_initialized.empty());
  ASSERT_EQ(string_initialized.size(), len);
  ASSERT_EQ(string_initialized, "bbbbbb");
  ASSERT_STREQ(string_initialized.c_str(), "bbbbbb");
}

/// @test After copying an `inline_string` from another `inline_string` with the same capacity, an `inline_string` with
/// a different capacity, a string literal, whether via copy-construction or assignment, the target `inline_string`
/// compares equal to the source. Where the source is an `inline_string`, invoking `c_str` on the source and destination
/// yields different pointers.
TEST(InlineStringCtors, CopyingAndAssignment) {
  constexpr auto buffer_size1 = 12;
  inline_string<buffer_size1> const str_1("helloxxxxxxz");
  inline_string<buffer_size1> str_2(str_1);

  ASSERT_EQ(str_1, str_2);
  ASSERT_NE(str_1.c_str(), str_2.c_str());

  str_2 = "goodbye";
  ASSERT_NE(str_1, str_2);
  ASSERT_EQ(str_2, "goodbye");

  constexpr auto buffer_size2 = 3;
  inline_string<buffer_size2> str_3("foo");
  inline_string<buffer_size1> str_4(str_3);

  ASSERT_EQ(str_4, str_3);

  str_2 = str_3;

  ASSERT_EQ(str_2, str_3);

  str_2 = str_1;
  ASSERT_EQ(str_2, str_1);

  str_4 = "goo";
  inline_string<buffer_size2> const str_5(str_4);

  ASSERT_EQ(str_4, str_5);

  str_3 = str_4;
  ASSERT_EQ(str_4, str_3);
}

/// @test The comparison operators on `inline_string` behave as expected for strings, including where the operands are
/// `inline_string` instances with different capacities.
TEST(InlineStringComparisonOperator, StringComparisons) {
  constexpr auto buffer_size1 = 42;
  constexpr auto buffer_size2 = 5;
  constexpr inline_string<buffer_size1> str_1("hello");
  constexpr inline_string<buffer_size1> str_2_const("goodbye");
  constexpr inline_string<buffer_size2> str_3("hello");
  constexpr inline_string<buffer_size2> str_4_const("hell");
  constexpr inline_string<buffer_size2> str_5("world");

  ASSERT_EQ(str_1, str_1);
  ASSERT_EQ(str_1, str_3);
  ASSERT_NE(str_1, str_5);
  ASSERT_NE(str_1, str_2_const);
  ASSERT_NE(str_2_const, str_1);
  ASSERT_EQ(str_2_const, str_2_const);
  ASSERT_NE(str_2_const, str_4_const);

  ASSERT_LT(str_1, str_5);
  ASSERT_LT(str_4_const, str_3);
  ASSERT_LT(str_2_const, str_4_const);
  ASSERT_LE(str_1, str_5);
  ASSERT_LE(str_4_const, str_3);
  ASSERT_LE(str_2_const, str_4_const);

  ASSERT_GT(str_5, str_1);
  ASSERT_GT(str_3, str_4_const);
  ASSERT_GT(str_4_const, str_2_const);
  ASSERT_GE(str_5, str_1);
  ASSERT_GE(str_3, str_4_const);
  ASSERT_GE(str_4_const, str_2_const);
}

/// @test `inline_string` instances can be compared with NUL-terminated strings, with the expected results for strings
TEST(InlineStringComparisonOperator, StringComparisonsCStrings) {
  inline_string<default_size> const inline_str_1("hello");
  constexpr inline_string<default_size> inline_str_2_const("goodbye");
  char const* const c_string_1("hello");
  char const* const c_string_3("hell");
  char const* const c_string_4("world");
  char const* const c_string_2("goodbye");

  ASSERT_EQ(c_string_1, inline_str_1);
  ASSERT_EQ(inline_str_1, c_string_1);
  ASSERT_NE(inline_str_1, c_string_4);
  ASSERT_NE(c_string_4, inline_str_1);
  ASSERT_NE(c_string_1, inline_str_2_const);
  ASSERT_NE(inline_str_2_const, c_string_1);
  ASSERT_EQ(inline_str_2_const, c_string_2);
  ASSERT_EQ(c_string_2, inline_str_2_const);
  ASSERT_NE(inline_str_2_const, c_string_3);

  ASSERT_TRUE(inline_str_1 == "hello");
  ASSERT_TRUE(inline_str_1 != "goodbye");

  ASSERT_EQ(inline_str_2_const, "goodbye");
  ASSERT_NE(inline_str_2_const, "hello");
  ASSERT_LT(inline_str_2_const, "hello");
  ASSERT_LE(inline_str_2_const, "hello");
  ASSERT_GT(inline_str_2_const, "bad");
  ASSERT_GE(inline_str_2_const, "bad");

  ASSERT_LT(inline_str_1, c_string_4);
  ASSERT_LT(c_string_2, inline_str_1);
  ASSERT_LT(inline_str_2_const, c_string_3);
  ASSERT_LE(inline_str_1, c_string_4);
  ASSERT_LE(c_string_2, inline_str_1);
  ASSERT_LE(inline_str_2_const, c_string_3);

  ASSERT_GT(c_string_4, inline_str_1);
  ASSERT_GT(inline_str_1, c_string_2);
  ASSERT_GT(c_string_3, inline_str_2_const);
  ASSERT_GE(c_string_4, inline_str_1);
  ASSERT_GE(inline_str_1, c_string_2);
  ASSERT_GE(c_string_3, inline_str_2_const);
}

/// @test The index operator on a `const` `inline_string` returns a `const` reference to the character
TEST(InlineStringIndexOperator, ReturnsConstReferenceToCharIfConst) {
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<inline_string<default_size> const&>().operator[](0)), char const&>();
}

/// @test The index operator on a non-`const` `inline_string` returns a non-`const` reference to the character
TEST(InlineStringIndexOperator, ReturnsReferenceToCharIfNonConst) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<inline_string<default_size>&>().operator[](0)), char&>();
}

/// @test The index operator returns the character with the appropriate index. If the index is equal to the size of the
/// string, the returned character is NUL.
TEST(InlineStringIndexOperator, ReturnsCharacterAtIndexConst) {
  constexpr auto* expected_string = "hello";
  constexpr inline_string<default_size> str{expected_string};
  ASSERT_EQ(str[0], 'h');
  ASSERT_EQ(str[1], 'e');
  ASSERT_EQ(str[str.size()], '\0');
}

/// @test The index operator for a non-`const` `inline_string` returns a reference to the character with the specified
/// index. Changing the characters via this reference changes the string.
TEST(InlineStringIndexOperator, ReturnsCharacterAtIndexNonConst) {
  inline_string<default_size> str{"hello"};
  str[0] = 'm';
  str[1] = 'a';
  EXPECT_EQ(str, "mallo");
}

/// @test `inline_string` iterators are random access
TEST(InlineStringIterator, IsRandomAccessIterator) {
  ASSERT_TRUE(arene::base::is_random_access_iterator_v<inline_string<default_size>::iterator>);
  ASSERT_TRUE(arene::base::is_random_access_iterator_v<inline_string<default_size>::const_iterator>);
}

/// @test The iterator returned from `begin` on an `inline_string` can be indexed to access the characters the same as
/// using the index operator on the string itself.
TEST(InlineStringIterator, RandomAccessFromIterator) {
  constexpr inline_string<default_size> str("hello");

  ASSERT_EQ(str[0], str.begin()[0]);
  ASSERT_EQ(str[1], str.begin()[1]);
  ASSERT_EQ(str[2], str.begin()[2]);
  ASSERT_EQ(str[3], str.begin()[3]);
  ASSERT_EQ(str[4], str.begin()[4]);
}

/// @test Accessing elements via `inline_string` iterators is consistent with accessing them via the index operator on
/// the string itself, or via the pointer returned from `data`
TEST(InlineStringIterator, ElementAccess) {
  constexpr inline_string<default_size> str("hello");

  ASSERT_EQ(*str.begin(), str[0]);
  ASSERT_EQ(str.begin().operator->(), str.data());

  inline_string<default_size> non_const_str{"hello"};
  (*non_const_str.begin()) = 'c';
  EXPECT_EQ(non_const_str[0], 'c');
  EXPECT_EQ(non_const_str.begin().operator->(), non_const_str.data());
}

/// @test Arithmetic operations on `inline_string` iterators behaves as expected, and allows access to the appropriate
/// characters of the string
TEST(InlineStringIterator, IteratorArithmetic) {
  inline_string<default_size> const str("hello world");

  auto iter = str.begin();
  auto iter2 = iter++;
  ASSERT_EQ(iter2, str.begin());
  ASSERT_NE(iter, iter2);
  ASSERT_EQ(&*iter, &str[1]);
  auto iter3 = iter + 2;
  ASSERT_EQ(&*iter3, &str[3]);
  auto iter4 = --iter3;
  auto iter5 = iter3--;
  ASSERT_EQ(iter4, iter5);
  ASSERT_EQ(&*iter3, &str[1]);
  ASSERT_EQ(&*iter4, &str[2]);
  auto iter6 = ++iter;
  ASSERT_EQ(iter4, iter6);
  ASSERT_EQ(iter4, iter);

  iter4 += 3;
  ASSERT_EQ(&*iter4, &str[5]);
  iter4 -= 2;
  ASSERT_EQ(&*iter4, &str[3]);
  ASSERT_EQ(iter4 - 1, str.begin() + 2);
  ASSERT_EQ(iter4, str.begin() + 3);
  ASSERT_EQ(iter4 - 1, 2 + str.begin());
  ASSERT_EQ(iter4, 3 + str.begin());
}

class itr_compare_and_convert {
 public:
  bool compare_non_const_to_const;
  bool compare_const_to_non_const;
  bool convert_non_const_to_const;
};

constexpr auto do_constexpr_iterator_convert_and_compare(inline_string<default_size> str) -> itr_compare_and_convert {
  using itr_type = inline_string<default_size>::iterator;
  using const_itr_type = inline_string<default_size>::const_iterator;
  itr_type const ncbegin = str.begin();
  const_itr_type const cbegin = str.cbegin();
  itr_type const ncend = str.end();
  const_itr_type const cend = str.cend();
  itr_compare_and_convert results{};
  results.compare_non_const_to_const = ncbegin == cbegin && ncbegin < cend && ncend > cbegin;
  results.compare_const_to_non_const = cbegin == ncbegin && cbegin < ncend && cend > ncbegin;
  results.convert_non_const_to_const = const_itr_type{ncbegin} == cbegin && const_itr_type{ncend} == cend;
  return results;
}

/// @test `iterator` values can be converted to `const_iterator`, and mixes of `iterator` and `const_iterator` into the
/// same `inline_string` can be compared.
TEST(InlineStringIterator, CanConvertAndCompareConstAndNonConstIterators) {
  constexpr itr_compare_and_convert results = do_constexpr_iterator_convert_and_compare("hello");
  ASSERT_TRUE(results.compare_non_const_to_const);
  ASSERT_TRUE(results.compare_const_to_non_const);
  ASSERT_TRUE(results.convert_non_const_to_const);
}

/// @test `begin`, `end`, `cbegin` and `cend` return appropriate iterators. These iterators allow iteration over the
/// characters in the string.
TEST(InlineStringIteration, CanIterateOverElements) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>&>().begin()),
      inline_string<default_size>::iterator>();
  ::testing::StaticAssertTypeEq<decltype(*std::declval<inline_string<default_size>&>().begin()), char&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>&>().end()),
      inline_string<default_size>::iterator>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size> const&>().begin()),
      inline_string<default_size>::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(*std::declval<inline_string<default_size> const&>().begin()), char const&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size> const&>().end()),
      inline_string<default_size>::const_iterator>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size> const&>().cbegin()),
      inline_string<default_size>::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(*std::declval<inline_string<default_size> const&>().cbegin()), char const&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size> const&>().cend()),
      inline_string<default_size>::const_iterator>();

  inline_string<default_size> str("hello");
  {
    SCOPED_TRACE("non-const begin,end");
    arene::base::array<char, 5> data{};
    arene::base::copy(str.begin(), str.end(), data.begin());
    EXPECT_TRUE(arene::base::equal(data.begin(), data.end(), str.begin(), str.end()));
  }
  {
    SCOPED_TRACE("const begin,end");
    auto const& const_str = str;
    arene::base::array<char, 5> data{};
    arene::base::copy(const_str.begin(), const_str.end(), data.begin());
    EXPECT_TRUE(arene::base::equal(data.begin(), data.end(), str.begin(), str.end()));
  }
  {
    SCOPED_TRACE("cbegin,cend");
    arene::base::array<char, 5> data{};
    arene::base::copy(str.cbegin(), str.cend(), data.begin());
    EXPECT_TRUE(arene::base::equal(data.begin(), data.end(), str.begin(), str.end()));
  }
}

/// @test The `reverse_iterator` and `const_reverse_iterator` instances returned from `rbegin` and `rend` or `crbegin`
/// and `crend` can be used to iterate through the characters in a string in reverse order.
TEST(InlineStringIteration, ReverseIteration) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>&>().rbegin()),
      ::arene::base::reverse_iterator<inline_string<default_size>::iterator>>();
  ::testing::StaticAssertTypeEq<decltype(*std::declval<inline_string<default_size>&>().rbegin()), char&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>&>().rend()),
      ::arene::base::reverse_iterator<inline_string<default_size>::iterator>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size> const&>().rbegin()),
      ::arene::base::reverse_iterator<inline_string<default_size>::const_iterator>>();
  ::testing::StaticAssertTypeEq<decltype(*std::declval<inline_string<default_size> const&>().rbegin()), char const&>();
  ::testing::StaticAssertTypeEq<decltype(*std::declval<inline_string<default_size> const&>().crbegin()), char const&>();

  inline_string<default_size> str("hello");
  {
    SCOPED_TRACE("non-const rbegin,rend");
    arene::base::array<char, 5> data{};
    arene::base::copy(str.rbegin(), str.rend(), data.begin());
    std::reverse(data.begin(), data.end());
    EXPECT_TRUE(arene::base::equal(data.begin(), data.end(), str.begin(), str.end()));
  }
  {
    SCOPED_TRACE("const rbegin,rend");
    auto const& const_str = str;
    arene::base::array<char, 5> data{};
    arene::base::copy(const_str.rbegin(), const_str.rend(), data.begin());
    std::reverse(data.begin(), data.end());
    EXPECT_TRUE(arene::base::equal(data.begin(), data.end(), str.begin(), str.end()));
  }
  {
    SCOPED_TRACE("crbegin,crend");
    arene::base::array<char, 5> data{};
    arene::base::copy(str.crbegin(), str.crend(), data.begin());
    std::reverse(data.begin(), data.end());
    EXPECT_TRUE(arene::base::equal(data.begin(), data.end(), str.begin(), str.end()));
  }
}

/// @test An `inline_string` constructed directly from a char array with embedded NUL characters contains the part
/// before the NUL, even if the `inline_string` has enough capacity to copy the whole array.
TEST(InlineString, EmbeddedNullsDirectConstruction) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const cstring_with_nulls[] = {'h', 'e', 'l', 'l', 'o', '\0', 'w', 'o', 'r', 'l', 'd'};
  inline_string<sizeof(cstring_with_nulls)> const inline_str_brace{"hello\0world"};
  inline_string<sizeof(cstring_with_nulls)> const inline_str_equals = cstring_with_nulls;

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wtautological-value-range-compare", "We want this assertion even if it ends up trivial");
  ASSERT_NE(inline_str_brace.size(), sizeof(cstring_with_nulls));
  ARENE_IGNORE_END();

  ASSERT_EQ(inline_str_brace.size(), 5U);
  ASSERT_EQ(inline_str_brace, &cstring_with_nulls[0]);
  ASSERT_EQ(inline_str_brace, inline_str_equals);
}

/// @test An `inline_string` can be brace-constructed from C-strings which are the exact correct size or smaller.
TEST(InlineString, ExactSizeCStringWithBraces) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const cstring[] = "hello";
  // NOLINTNEXTLINE(hicpp-no-array-decay)
  arene::base::inline_string<sizeof(cstring)> const bigger_from_array{cstring};
  arene::base::inline_string<sizeof(cstring)> const bigger_from_literal{"hello"};
  ASSERT_EQ(bigger_from_array.size(), sizeof(cstring) - 1U);
  ASSERT_EQ(bigger_from_array, bigger_from_literal);
  ASSERT_EQ(bigger_from_array, cstring);

  // NOLINTNEXTLINE(hicpp-no-array-decay)
  arene::base::inline_string<sizeof(cstring) - 1U> const exact_from_array{cstring};
  arene::base::inline_string<sizeof(cstring) - 1U> const exact_from_literal{"hello"};
  ASSERT_EQ(exact_from_array.size(), sizeof(cstring) - 1U);
  ASSERT_EQ(exact_from_array, exact_from_literal);
  ASSERT_EQ(exact_from_array, cstring);

  ASSERT_EQ(bigger_from_array.size(), exact_from_array.size());
  ASSERT_EQ(bigger_from_array, exact_from_array);
}

/// @test An `inline_string` can be equals-constructed from C-strings which are the exact correct size or smaller.
TEST(InlineString, ExactSizeCStringWithEquals) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const cstring[] = "hello";
  arene::base::inline_string<sizeof(cstring)> const bigger = cstring;
  ASSERT_EQ(bigger.size(), sizeof(cstring) - 1U);
  ASSERT_EQ(bigger, cstring);

  arene::base::inline_string<sizeof(cstring) - 1U> const exact = cstring;
  ASSERT_EQ(bigger.size(), sizeof(cstring) - 1U);
  ASSERT_EQ(exact, cstring);

  ASSERT_EQ(bigger.size(), exact.size());
  ASSERT_EQ(bigger, exact);
}

/// @test An `inline_string` can be equals-constructed from an empty C-string
TEST(InlineString, EmptyCString) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const cstring[] = "";
  arene::base::inline_string<default_size> const empty = cstring;
  ASSERT_EQ(empty.size(), 0UL);
  ASSERT_EQ(empty, cstring);
}

/// Constexpr wrappers around inline_string<N>::append()
/// @returns A copy of the input string that has had `append(args...)` called on it.
/// @{
template <typename Rhs>
constexpr auto do_append(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs.append(rhs);
  return lhs;
}

constexpr auto do_append(inline_string<default_size> lhs, string_view rhs, inline_string<default_size>::size_type pos)
    -> inline_string<default_size> {
  lhs.append(rhs, pos);
  return lhs;
}

constexpr auto do_append(
    inline_string<default_size> lhs,
    string_view rhs,
    inline_string<default_size>::size_type pos,
    inline_string<default_size>::size_type count
) -> inline_string<default_size> {
  lhs.append(rhs, pos, count);
  return lhs;
}

constexpr auto do_append(inline_string<default_size> lhs, inline_string<default_size>::size_type count, char chr)
    -> inline_string<default_size> {
  lhs.append(count, chr);
  return lhs;
}
/// @}

/// @test Invoking the `append` member function with a string literal increases the string length by the length of the
/// string literal, and copies the characters from the string literal onto the end of the string.
TEST(InlineStringAppend, StringLiteralAtEndOfString) { ASSERT_EQ(do_append("hello", " world"), "hello world"); }

/// @test Invoking the `append` member function with a pointer to a NUL-terminated string increases the string length by
/// the length of the NUL-terminated string, and copies the characters from the NUL-terminated string onto the end of
/// the string.
TEST(InlineStringAppend, CStringAtEndOfString) {
  constexpr auto* world = " world";
  ASSERT_EQ(do_append("hello", world), "hello world");
}

/// @test Invoking the `append` member function with a `string_view` increases the string length by the length of the
/// `string_view`, and copies the characters from the `string_view` onto the end of the string.
TEST(InlineStringAppend, StringViewAtEndOfString) {
  ASSERT_EQ(do_append("hello", string_view{" world"}), "hello world");
}

/// @test Invoking `append` with a `string_view` and a position appends the substring of that `string_view` starting at
/// the specified position on to the end of the string.
TEST(InlineStringAppend, StringViewPosAtEndOfString) {
  constexpr string_view world{" world"};
  ASSERT_EQ(do_append("hello", world, 1), "helloworld");
  ASSERT_EQ(do_append("hello", world, world.size()), "hello");
}

/// @test Invoking `append` with a `string_view` a position and a count appends the substring of that `string_view` to
/// the specified length starting at the specified position on to the end of the string.
TEST(InlineStringAppend, StringViewPosCountAtEndOfString) {
  constexpr string_view world{" world"};
  ASSERT_EQ(do_append("hello", world, 1, 2), "hellowo");
  ASSERT_EQ(do_append("hello", world, world.size(), world.size()), "hello");
}

/// @test Invoking `append` with a single character appends that character to the string
TEST(InlineStringAppend, CharAtEndOfString) { ASSERT_EQ(do_append("hello", 'x'), "hellox"); }

/// @test Invoking `append` with a count and a single character appends the specified number of copies of that character
/// to the string
TEST(InlineStringAppend, CharRepeatingAtEndOfString) { ASSERT_EQ(do_append("hello", 5, 'x'), "helloxxxxx"); }

/// @test Invoking `append` with an `inline_string` appends the contents of that string to the `inline_string` on which
/// `append` was invoked.
TEST(InlineStringAppend, InlineStringThatDefinitelyFitsAtEndOfString) {
  ASSERT_EQ(do_append("hello", (inline_string<default_size>{" world"})), "hello world");
}

/// @test Invoking `append` with an `inline_string` appends the contents of that string to the `inline_string` on which
/// `append` was invoked, even if the capacity of the appended string is large.
TEST(InlineStringAppend, InlineStringMaybeFitsAtEndOfStringIfSizeFitsInRemainingCapacity) {
  ASSERT_EQ(do_append("hello", (inline_string<default_size + 10>{" world"})), "hello world");
}

/// @test Invoking `append` with an initializer list of characters appends the characters from that initializer list to
/// the `inline_string` on which `append` was invoked.
TEST(InlineStringAppend, InitializerListAtEndOfString) {
  ASSERT_EQ(do_append("hello", std::initializer_list<char>{' ', 'w', 'o', 'r', 'l', 'd'}), "hello world");
}

/// Constexpr wrapper around inline_string<N>::+=()
/// @returns A copy of the input string that has had `+= rhs` called on it.
template <typename Rhs>
constexpr auto do_plus_assign(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs += rhs;
  return lhs;
}

/// @test Using `+=` to append a string literal to an `inline_string` adds the characters to the `inline_string`
TEST(InlineStringPlusAssign, StringLiteralAtEndOfString) {
  ASSERT_EQ(do_plus_assign("hello", " world"), "hello world");
}

/// @test Using `+=` to append a pointer to a NUL-terminated string to an `inline_string` adds the characters from the
/// NUL-terminated string to the `inline_string`
TEST(InlineStringPlusAssign, CStringAtEndOfString) {
  constexpr auto* world = " world";
  ASSERT_EQ(do_plus_assign("hello", world), "hello world");
}

/// @test Using `+=` to append a `string_view` to an `inline_string` adds the characters from the `string_view` to the
/// `inline_string`
TEST(InlineStringPlusAssign, StringViewAtEndOfString) {
  ASSERT_EQ(do_plus_assign("hello", string_view{" world"}), "hello world");
}

/// @test Using `+=` to append a character to an `inline_string` adds that character to the end of the string
TEST(InlineStringPlusAssign, CharAtEndOfString) { ASSERT_EQ(do_plus_assign("hello", 'x'), "hellox"); }

/// @test Using `+=` to append an `inline_string` to an `inline_string` adds the characters from the `inline_string`
/// that is the RHS operand to the  `inline_string` that is the LHS operand
TEST(InlineStringPlusAssign, InlineStringThatDefinitelyFitsAtEndOfString) {
  ASSERT_EQ(do_plus_assign("hello", (inline_string<default_size>{" world"})), "hello world");
}

/// @test Using `+=` to append an `inline_string` with a larger capacity to an `inline_string` adds the characters from
/// the `inline_string` that is the RHS operand to the  `inline_string` that is the LHS operand
TEST(InlineStringPlusAssign, InlineStringMaybeFitsAtEndOfStringIfSizeFitsInRemainingCapacity) {
  ASSERT_EQ(do_plus_assign("hello", (inline_string<default_size + 10>{" world"})), "hello world");
}

/// @test Using `+=` to append an initializer list of characters to an `inline_string` adds the characters from the
/// initializer list to the `inline_string`
TEST(InlineStringPlusAssign, InitializerListAtEndOfString) {
  ASSERT_EQ(do_plus_assign("hello", std::initializer_list<char>{' ', 'w', 'o', 'r', 'l', 'd'}), "hello world");
}

/// Constexpr wrapper for inline_string<default_size>.push_back()
/// @return The input string with push_back(arg) called on it.
constexpr auto do_push_back(inline_string<default_size> lhs, char a_char) -> inline_string<default_size> {
  lhs.push_back(a_char);
  return lhs;
}

/// @test Invoking `push_back` appends a character to an `inline_string`
TEST(InlineStringPushBack, AppendsCharToEndOfString) { ASSERT_EQ(do_push_back("hello", 'Z'), "helloZ"); }

/// constexpr wrapper for inline_string<default_size>::operator=
/// @return A copy of the input string with rhs assigned to it.
template <typename Rhs>
constexpr auto do_assignment(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs = rhs;
  return lhs;
}

/// @test Assigning to an `inline_string` from a single character replaces the string with a string of length one
/// holding that character
TEST(InlineStringAssignment, CharReplacesStringWithRhs) { ASSERT_EQ(do_assignment("hello", 'a'), "a"); }

/// @test Assigning to an `inline_string` from a string literal replaces the string with a string with the same length
/// and characters as the string literal
TEST(InlineStringAssignment, StringLiteralReplacesStringWithRhs) {
  ASSERT_EQ(do_assignment("hello", "goodbye"), "goodbye");
}

/// @test Assigning to an `inline_string` from a pointer to a NUL-terminated string replaces the string with a string
/// with the same length and characters as the NUL-terminated string
TEST(InlineStringAssignment, CStrReplacesStringWithRhs) {
  constexpr auto* goodbye = "goodbye";
  ASSERT_EQ(do_assignment("hello", goodbye), goodbye);
}

/// @test Assigning to an `inline_string` from a `string_view` replaces the string with a string with the same length
/// and characters as the `string_view`
TEST(InlineStringAssignment, StringViewReplacesStringWithRhs) {
  constexpr string_view goodbye{"goodbye"};
  ASSERT_EQ(do_assignment("hello", goodbye), goodbye);
}

/// @test Assigning to an `inline_string` from an initializer list of characters replaces the string with a string
/// with the same length and characters as the initializer list
TEST(InlineStringAssignment, InitializerListReplacesStringWithRhs) {
  ASSERT_EQ(do_assignment("hello", std::initializer_list<char>{'a', 'b', 'c'}), "abc");
}

/// @test Invoking `capacity` returns the capacity of the string
TEST(InlineStringCapacity, IsMaxLength) {
  constexpr unsigned size = 123;
  constexpr inline_string<size> str;
  ASSERT_EQ(str.capacity(), size);
}

/// Constexpr around inline_string<default_size>.resize()
/// @return A copy of the input string with `resize(args..)` called on it.
/// @{
constexpr auto do_resize(inline_string<default_size> str, inline_string<default_size>::size_type new_size)
    -> inline_string<default_size> {
  str.resize(new_size);
  return str;
}
constexpr auto do_resize(inline_string<default_size> str, inline_string<default_size>::size_type new_size, char chr)
    -> inline_string<default_size> {
  str.resize(new_size, chr);
  return str;
}
/// @}

/// @test Invoking `resize` with a size of zero sets the string to an empty string
TEST(InlineStringResize, ToZeroIsEmptyString) {
  ASSERT_TRUE(do_resize("hello", 0).empty());
  ASSERT_TRUE(do_resize("hello world", 0).empty());
}

/// @test Invoking `resize` with a size less than or equal to the capacity (with or without a fill character) sets the
/// length of the string to the new size.
TEST(InlineStringResize, ToNotGreaterThanMaxLengthResultsInSize) {
  ASSERT_EQ(do_resize("hello", 1).size(), 1);
  ASSERT_EQ(do_resize("hello", 1, 'a').size(), 1);

  ASSERT_EQ(do_resize("hello", 4).size(), 4);
  ASSERT_EQ(do_resize("hello", 4, 'a').size(), 4);

  ASSERT_EQ(do_resize("hello", default_size).size(), default_size);
  ASSERT_EQ(do_resize("hello", default_size, 'a').size(), default_size);
}

/// @test Invoking `resize` with a size less than the current size (with or without a fill character) truncates the
/// existing string to the specified length
TEST(InlineStringResize, IfLessThanOrEqualCurrentSizeTruncatesToNewSize) {
  constexpr inline_string<default_size> original{"hello"};
  ASSERT_EQ(do_resize(original, 1), original.substr(0, 1));
  ASSERT_EQ(do_resize(original, 1, 'a'), original.substr(0, 1));
  ASSERT_EQ(do_resize(original, original.size()), original.substr(0, original.size()));
  ASSERT_EQ(do_resize(original, original.size(), 'a'), original.substr(0, original.size()));
}

/// @test Invoking `resize` with a size larger than the current size without a fill character extends the string to the
/// new size by padding with NUL characters
TEST(InlineStringResize, IfGreaterThanCurrentSizeAndNoCharFillsRemainderWithNull) {
  constexpr inline_string<default_size> original{"hello"};
  ASSERT_STREQ(do_resize(original, original.size() + 1).c_str(), original.c_str());
  ASSERT_EQ(do_resize(original, original.size() + 1)[original.size()], '\0');
  ASSERT_EQ(do_resize(original, original.size() + 10)[original.size() + 10], '\0');
}

/// @test Invoking `resize` with a size larger than the current size with a fill character extends the string to the
/// new size by padding with the specified fill character
TEST(InlineStringResize, IfGreaterThanCurrentSizeFillsRemainderWithChar) {
  constexpr inline_string<default_size> original{"hello"};
  ASSERT_EQ(do_resize(original, original.size() + 1, 'a'), original + string_view{"a"});
  ASSERT_EQ(do_resize(original, original.size() + 5, 'a'), original + string_view{"aaaaa"});
}

/// @test Coverting an `inline_string` to a `string_view` sets the `data` and `size` of the `string_view` to the results
/// of `c_str` and `size` on the `inline_string`
TEST(InlineStringConversion, StringViewConversion) {
  constexpr inline_string<default_size> str{"hello"};
  ASSERT_EQ(static_cast<string_view>(str).data(), str.c_str());
  ASSERT_EQ(static_cast<string_view>(str).size(), str.size());
}

/// Constexpr wrapper for inline_string<default_size>.clear()
/// @return The input string with .clear() called on it.
constexpr auto do_clear(inline_string<default_size> str) -> inline_string<default_size> {
  str.clear();
  return str;
}

/// @test Invoking `clear` changes the string to an empty string
TEST(InlineStringClear, ResultsInEmptyString) { ASSERT_TRUE(do_clear("hello").empty()); }

/// @test `length` and `size` return the same value
TEST(InlineStringLength, IsSameAsSize) {
  constexpr inline_string<default_size> str{"hello"};
  ASSERT_EQ(str.length(), str.size());
}

/// @test 'front' returns a reference to the first character in the string
TEST(InlineStringFront, GetsFirstCharacter) {
  constexpr inline_string<default_size> str{"hello"};
  ::testing::StaticAssertTypeEq<decltype(str.front()), char const&>();
  ASSERT_EQ(str.front(), str[0]);
  ASSERT_EQ(&str.front(), str.data());
}

/// @test `back` returns a reference to the last character in the string
TEST(InlineStringBack, GetsLastCharacter) {
  constexpr inline_string<default_size> str{"hello"};
  ::testing::StaticAssertTypeEq<decltype(str.back()), char const&>();
  ASSERT_EQ(str.back(), str[str.size() - 1]);
  ASSERT_EQ(&str.back(), &str[str.size() - 1]);
}

/// @test Assigning one string to another using `assign` sets the destination string to be equal to the source string
TEST(InlineString, Assignment) {
  inline_string<default_size> const str{"hello"};
  inline_string<default_size> str_2;
  auto& str_3 = str_2.assign(str);
  ASSERT_EQ(str_2, "hello");
  ASSERT_EQ(&str_3, &str_2);
}

/// @test Invoking `assign` with a pointer to a NUL-terminated string sets the destination string to be equal to the
/// NUL-terminated string
TEST(InlineString, AssignmentFromCharPointer) {
  char const* c_string{"hello"};
  inline_string<default_size> inline_str;
  auto& str_3 = inline_str.assign(c_string);
  ASSERT_EQ(inline_str, "hello");
  ASSERT_EQ(&str_3, &inline_str);
}

/// @test Invoking `assign` with a single character sets the destination string to be a string of length one, which is
/// the specified character
TEST(InlineString, AssignmentFromChar) {
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign('c');
  ASSERT_EQ(inline_str, "c");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

/// @test Invoking `assign` with a `string_view` and an offset sets the destination string to be equal to the substring
/// of that `string_view` starting at the specified offset
TEST(InlineString, AssignmentFromStringViewWithOffset) {
  string_view const view{"hello"};
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign(view, 2);
  ASSERT_EQ(inline_str, "llo");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

/// @test Invoking `assign` with a `string_view` an offset and a length, sets the destination string to be equal to the
/// substring of that `string_view` starting at the specified offset up to the specified length
TEST(InlineString, AssignmentFromStringViewWithOffsetAndLength) {
  string_view const view{"hello world"};
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign(view, 2, 5);
  ASSERT_EQ(inline_str, "llo w");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

/// @test Invoking `assign` with a character pointer and a length, sets the destination string to be equal to the
/// specified number of characters starting at that character pointer
TEST(InlineString, AssignmentFromCharPointerAndLength) {
  char const* char_ptr = "xxxxxyyyyy";
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign(char_ptr, 5);
  ASSERT_EQ(inline_str, "xxxxx");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

/// @test Invoking `assign` with a count and a character, sets the destination string to be equal to a string of the
/// specified length consisting of the specified number of copies of the character
TEST(InlineString, AssignmentFromCharAndCount) {
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign(6, 'a');
  ASSERT_EQ(inline_str, "aaaaaa");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

/// @test Invoking `assign` with an initializer list sets the destination string to have the same characters and length
/// as the initializer list
TEST(InlineString, AssignmentFromInitializerList) {
  inline_string<default_size> inline_str;
  auto& inline_str_2 = inline_str.assign({'a', 'b', 'c'});
  ASSERT_EQ(inline_str, "abc");
  ASSERT_EQ(&inline_str_2, &inline_str);
}

template <std::size_t MaxLength, typename... Args>
constexpr auto constexpr_assign(inline_string<MaxLength> inline_str, Args&&... args) -> inline_string<MaxLength> {
  return inline_str.assign(std::forward<Args>(args)...);
}

/// @test Invoking `insert` with an offset and a `string_view` inserts the characters from the string view at the
/// specified offset.
TEST(InlineStringInsert, StringView) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(3, string_view{"XYZ"});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abcXYZdef");
}

/// @test Invoking `insert` with an offset of zero and a `string_view` inserts the characters from the string view at
/// the start of the string
TEST(InlineStringInsert, StringViewAtBeginning) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(0, string_view{"XYZ"});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "XYZabcdef");
}

/// @test Invoking `insert` with an offset equal to the size of the string and a `string_view` inserts the characters
/// from the string view at the end of the string
TEST(InlineStringInsert, StringViewAtEnd) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(inline_str.size(), string_view{"XYZ"});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abcdefXYZ");
}

/// @test Invoking `insert` with an offset and a `string_view` that is longer than the original string inserts the
/// characters from the string view at the specified offset.
TEST(InlineStringInsert, LongerStringView) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(2, string_view{"XXXXYYYYZZZZ"});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abXXXXYYYYZZZZcdef");
}

/// @test Invoking `insert` with an offset, a `string_view` and a position, inserts the substring of the string view
/// starting at the specified position into the destination string at the specified offset.
TEST(InlineStringInsert, SubstringWithPos) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(2, string_view{"XXXXYYYYZZZZ"}, 4);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abYYYYZZZZcdef");
}

/// @test Invoking `insert` with an offset, a `string_view`, a position, and a length, inserts the substring of the
/// string view starting at the specified position up to the specified length, into the destination string at the
/// specified offset.
TEST(InlineStringInsert, SubstringWithPosAndLength) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(2, string_view{"XXXXYYYYZZZZ"}, 4, 6);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abYYYYZZcdef");
}

/// @test Invoking `insert` with an offset, a character pointer and a length, inserts the specified number of characters
/// from the character pointer into the destination string at the specified offset
TEST(InlineStringInsert, SubstringWithCharPointerAndLength) {
  inline_string<default_size> inline_str{"abcdef"};
  char const* c_string = "XXXXYYYYZZZZ";
  auto& inline_str_2 = inline_str.insert(2, c_string, 4);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abXXXXcdef");
}

/// @test Invoking `insert` with an offset, a count and a character, inserts the specified number of copies of the
/// specified character at the specified position into the destination string at the specified offset.
TEST(InlineStringInsert, NChars) {
  inline_string<default_size> inline_str{"abcdef"};
  auto& inline_str_2 = inline_str.insert(2, 6, 'Z');
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abZZZZZZcdef");
}

/// constexpr wrapper around srt.insert
/// @return str.insert(args...);
template <typename... Args>
constexpr auto do_insert(inline_string<default_size> str, Args&&... args) -> inline_string<default_size> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  return str.insert(std::forward<Args>(args)...);
}

/// @test Invoking `insert` with an iterator and a character inserts the specified character at the position referenced
/// by the iterator
TEST(InlineStringInsert, CharAtIteratorPosition) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;
  inline_string<default_size>::iterator const res = inline_str.insert(pos, 'X');
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abXcdef");
}

/// @test Invoking `insert` with an iterator and a count and a character inserts the specified number of copies of the
/// specified character at the position referenced by the iterator
TEST(InlineStringInsert, NCharsAtIteratorPosition) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;
  inline_string<default_size>::iterator const res = inline_str.insert(pos, 5, 'X');
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abXXXXXcdef");
}

/// @test Invoking `insert` with an iterator and an initializer list inserts the characters from the initializer list at
/// the position referenced by the iterator
TEST(InlineStringInsert, InitListAtIteratorPosition) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;
  inline_string<default_size>::iterator const res = inline_str.insert(pos, {'X', 'Y', 'Z'});
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abXYZcdef");
}

/// @test Invoking `insert` with an iterator and an iterator range denoted by forward iterators inserts the characters
/// from the iterator range at the position referenced by the iterator
TEST(InlineStringInsert, FromIteratorRange) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;

  using forward_it = testing::demoted_iterator<arene::base::array<char, 3>::const_iterator, std::forward_iterator_tag>;
  arene::base::array<char, 3> const source{'X', 'Y', 'Z'};
  forward_it const forward_begin{source.begin()};
  forward_it const forward_end{source.end()};

  inline_string<default_size>::iterator const res = inline_str.insert(pos, forward_begin, forward_end);
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abXYZcdef");
}

/// @test Invoking `insert` with an iterator and an iterator range denoted by bidrectional iterators inserts the
/// characters from the iterator range at the position referenced by the iterator
TEST(InlineStringInsert, BidrectionalIteratorRange) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;

  using forward_it =
      testing::demoted_iterator<arene::base::array<char, 3>::const_iterator, std::bidirectional_iterator_tag>;
  arene::base::array<char, 3> const source{'X', 'Y', 'Z'};
  forward_it const bidir_begin{source.begin()};
  forward_it const bidir_end{source.end()};

  inline_string<default_size>::iterator const res = inline_str.insert(pos, bidir_begin, bidir_end);
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abXYZcdef");
}

/// @test Invoking `insert` with an iterator and an iterator range denoted by random access iterators inserts the
/// characters from the iterator range at the position referenced by the iterator
TEST(InlineStringInsert, RandomAccessIteratorRange) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;
  arene::base::array<char, 3> const source{'X', 'Y', 'Z'};
  inline_string<default_size>::iterator const res = inline_str.insert(pos, source.begin(), source.end());
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abXYZcdef");
  ASSERT_STREQ(inline_str.c_str(), "abXYZcdef");
}

/// @test Invoking `erase` with a position removes the characters from that position to the end of the string
TEST(InlineStringErase, EraseFromPosition) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.erase(3);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abc");
}

/// @test Invoking `erase` with a position that is larger than the size of the string does nothing
TEST(InlineStringErase, EraseFromPositionGreaterThanSizeIsANoOp) {
  inline_string<default_size> inline_str{"abcdefg"};
  ASSERT_EQ(inline_str.erase(inline_str.size() * 2), inline_str);
}

/// @test Invoking `erase` with a position that is large enough to overflow does nothing
TEST(InlineStringErase, EraseWithOverflowingPositionIsANoOp) {
  inline_string<default_size> inline_str{"abcdefg"};
  ASSERT_EQ(inline_str.erase(std::numeric_limits<std::size_t>::max() - 2), inline_str);
}

/// @test Invoking `erase` without a position erases the entire string, so the string is empty
TEST(InlineStringErase, EraseWithoutPositionErasesEverything) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.erase();
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "");
  ASSERT_STREQ(inline_str.c_str(), "");
}

/// @test Invoking `erase` with a position and a length removes the specified number of characters from the string,
/// starting at that position
TEST(InlineStringErase, EraseWithPositionAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};

  auto& inline_str_2 = inline_str.erase(3, 2);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abcfg");
  ASSERT_STREQ(inline_str.c_str(), "abcfg");
}

/// @test Invoking `erase` with a position and a length that goes beyond the end of the string removes all characters to
/// the end of the string
TEST(InlineStringErase, EraseWithPositionAndExcessLength) {
  inline_string<default_size> inline_str{"abcdefg"};

  auto& inline_str_2 = inline_str.erase(3, decltype(inline_str)::capacity() + 1);
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abc");
  ASSERT_STREQ(inline_str.c_str(), "abc");
}

/// @test Invoking `erase` with a position outside the string is a no-op even if the length would overflow
TEST(InlineStringErase, EraseWithOverflowingPositionAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};

  auto& inline_str_2 = inline_str.erase(std::numeric_limits<std::size_t>::max() - 2U, 5U);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abcdefg");
  ASSERT_STREQ(inline_str.c_str(), "abcdefg");
}

/// @test Invoking `erase` with a position and a length that goes beyond the end of the string removes all characters to
/// the end of the string, even if the length is large enough to overflow
TEST(InlineStringErase, EraseWithPositionAndOverflowingLength) {
  inline_string<default_size> inline_str{"abcdefg"};

  auto& inline_str_2 = inline_str.erase(3, std::numeric_limits<std::size_t>::max());
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abc");
  ASSERT_STREQ(inline_str.c_str(), "abc");
}

/// @test Invoking `erase` with a position and a length when the position goes beyond the end of the string removes all
/// characters to the end of the string, even if the length is large enough to overflow
TEST(InlineStringErase, EraseWithOutOfBoundsPositionAndOverflowingLength) {
  inline_string<default_size> inline_str{"abcdefg"};

  auto& inline_str_2 = inline_str.erase(50'000, std::numeric_limits<std::size_t>::max() - 49'999);
  ASSERT_EQ(&inline_str_2, &inline_str);
  ASSERT_EQ(inline_str, "abcdefg");
  ASSERT_STREQ(inline_str.c_str(), "abcdefg");
}

template <std::size_t MaxLength, typename... Args>
constexpr auto constexpr_erase(inline_string<MaxLength> str, Args&&... args) -> inline_string<MaxLength> {
  return str.erase(std::forward<Args>(args)...);
}

/// @test Invoking `max_size` returns the capacity of the string
TEST(InlineString, MaxSizeEqualsCapacity) {
  constexpr auto size1 = 42;
  ASSERT_EQ(inline_string<size1>().max_size(), inline_string<size1>().capacity());
  ASSERT_EQ(inline_string<size1>::max_size(), inline_string<size1>::capacity());
}

/// @test Invoking `erase` with an iterator erases the character referenced by that iterator
TEST(InlineStringErase, EraseWithIteratorPosition) {
  inline_string<default_size> inline_str{"abcdef"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;
  inline_string<default_size>::iterator const res = inline_str.erase(pos);
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abdef");
  ASSERT_STREQ(inline_str.c_str(), "abdef");
}

/// @test Invoking `erase` with an iterator range erases the characters referenced by that iterator range
TEST(InlineStringErase, EraseWithIteratorRange) {
  inline_string<default_size> inline_str{"abcdefghijkl"};
  auto const& const_inline_str = inline_str;
  auto const pos = const_inline_str.begin() + 2;
  auto const end = pos + 3;
  inline_string<default_size>::iterator const res = inline_str.erase(pos, end);
  ASSERT_EQ(res, pos);
  ASSERT_EQ(inline_str, "abfghijkl");
  ASSERT_STREQ(inline_str.c_str(), "abfghijkl");
}

template <std::size_t MaxLength, typename... Args>
constexpr auto constexpr_iterator_erase(inline_string<MaxLength> str, Args&&... args) -> inline_string<MaxLength> {
  str.erase((str.begin() + std::forward<Args>(args))...);
  return str;
}

/// @test Invoking `pop_back` removes the last character in the string
TEST(InlineStringPopBack, PopbackRemovesLastChar) {
  inline_string<default_size> inline_str{"hello"};
  inline_str.pop_back();
  ASSERT_EQ(inline_str, "hell");
  ASSERT_STREQ(inline_str.c_str(), "hell");
}

template <std::size_t MaxLength>
constexpr auto constexpr_pop_back(inline_string<MaxLength> str) -> inline_string<MaxLength> {
  str.pop_back();
  return str;
}

/// @test `npos` is equal to the max value representable by `std::size_t`
TEST(InlineString, Npos) {
  ::testing::StaticAssertTypeEq<decltype(inline_string<default_size>::npos), std::size_t const>();
  ASSERT_EQ(inline_string<default_size>::npos, std::numeric_limits<std::size_t>::max());
}

/// @test Invoking `replace` with a length of zero and an empty view does nothing
TEST(InlineStringReplace, EmptyRangeWithEmptyView) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(3, 0, string_view{});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdefg");
  ASSERT_STREQ(inline_str.c_str(), "abcdefg");
}

/// @test Invoking `replace` with a length of zero and a non-empty `string_view` is equivalent to invoking `insert` with
/// the specified position
TEST(InlineStringReplace, ReplaceEmptyRangeWithViewIsInsert) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(3, 0, string_view{"XYZ"});
  ASSERT_EQ(inline_str_2, "abcXYZdefg");
  ASSERT_STREQ(inline_str.c_str(), "abcXYZdefg");
}

/// @test Invoking `replace` with a position and length and a non-empty `string_view` removes the specified number of
/// characters from the original string and replaces them with the characters from the view, making the string larger if
/// necessary
TEST(InlineStringReplace, CanReplaceNonEmptyRange) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, string_view{"XYZ"});
  ASSERT_EQ(inline_str_2, "abcdXYZg");
  ASSERT_STREQ(inline_str.c_str(), "abcdXYZg");
}

/// @test Invoking `replace` with a `string_view` the same length as the removed length replaces the characters
TEST(InlineStringReplace, AllowsInsertionOfEqualSize) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(3, 3, string_view{"XYZ"});
  ASSERT_EQ(inline_str_2, "abcXYZg");
}

/// @test Invoking `replace` with an offset of 0 replaces the characters at the beginning of the string
TEST(InlineStringReplace, AtBeginning) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(0, 2, string_view{"XYZ"});
  ASSERT_EQ(inline_str_2, "XYZcdefg");
}

/// @test Invoking `replace` with an offset equal to the length of the existing string appends the new characters
TEST(InlineStringReplace, AtEnd) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(inline_str.size(), 2, string_view{"XYZ"});
  ASSERT_EQ(inline_str_2, "abcdefgXYZ");
}

/// @test Invoking `replace` with a `string_view` that is shorter than the specified removal length shrinks the string
/// as appropriate
TEST(InlineStringReplace, ReplaceLargeChunkWithSmall) {
  inline_string<default_size> inline_str{"abcdXXXXXXXXXXXXXXXXXXXXefg"};
  auto& inline_str_2 = inline_str.replace(4, 20, string_view{"XYZ"});
  ASSERT_EQ(inline_str_2, "abcdXYZefg");
}

/// @test Invoking `replace` with a `string_view` that is larger than the specified removal length inserts the
/// additional characters
TEST(InlineStringReplace, ReplaceSmallChunkWithLarge) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, string_view{"XXXXXXXXXXXXXXXXXXXX"});
  ASSERT_EQ(inline_str_2, "abcdXXXXXXXXXXXXXXXXXXXXg");
}

/// @test Invoking `replace` with a `string_view` and an offset replaces the specified characters with the substring of
/// the `string_view` starting at the specified offset
TEST(InlineStringReplace, ReplaceWithSubstr) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, string_view{"AAAXX"}, 3);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXXg");
}

/// @test Invoking `replace` with a `string_view`, an offset and a length replaces the specified characters with the
/// substring of the `string_view` starting at the specified offset up to the specified length
TEST(InlineStringReplace, ReplaceWithSubstrWithPosAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, string_view{"AAAXXYYZZZ"}, 3, 4);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXXYYg");
}

/// @test Invoking `replace` with a character pointer and a length replaces the specified characters with the specified
/// number of characters from the character pointer
TEST(InlineStringReplace, ReplaceWithCharPointerAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, static_cast<char const*>("AAAXXYYZZZ"), 3);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdAAAg");
}

/// @test Invoking `replace` with a pointer to a NUL-terminated string replaces the specified characters with the
/// characters from the NUL-terminated string
TEST(InlineStringReplace, ReplaceWithCharPointer) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, static_cast<char const*>("AAAXXYYZZZ"));
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdAAAXXYYZZZg");
}

/// @test Invoking `replace` with a count and a character replaces the specified characters with the specified number of
/// copies of the specified character
TEST(InlineStringReplace, ReplaceWithCharAndCount) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto& inline_str_2 = inline_str.replace(4, 2, 5, 'S');
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdSSSSSg");
}

/// constexpr wrapper for inline_string.replace
/// @return str.replace(args...)
template <typename... Args>
constexpr auto do_replace(inline_string<default_size> str, Args&&... args) -> inline_string<default_size> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  return str.replace(std::forward<Args>(args)...);
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and a `string_view` replaces the
/// characters in the range with those from the `string_view`
TEST(InlineStringReplace, ReplaceIteratorRange) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 =
      inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, string_view{"XYZ"});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXYZg");
  ASSERT_STREQ(inline_str.c_str(), "abcdXYZg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and a `string_view` and offset
/// replaces the characters in the range with those from the substring of the `string_view` starting at the specified
/// offset
TEST(InlineStringReplace, ReplaceIteratorRangeWithSVPlusOffset) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 =
      inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, string_view{"XXXYYYY"}, 3);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdYYYYg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and a `string_view` and offset and
/// length replaces the characters in the range with those from the substring of the `string_view` starting at the
/// specified offset, up to the specified length
TEST(InlineStringReplace, ReplaceIteratorRangeWithSVPlusOffsetAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 =
      inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, string_view{"XXXYYYYZZZZ"}, 3, 4);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdYYYYg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and a pointer to a NUL-terminated
/// string replaces the characters in the range with those from the NUL-terminated string
TEST(InlineStringReplace, ReplaceIteratorRangeCharPointer) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 =
      inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, static_cast<char const*>("XYZ"));
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXYZg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and a character pointer and length
/// replaces the characters in the range with the specified number of characters from the character pointer
TEST(InlineStringReplace, ReplaceIteratorRangeCharPointerAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 =
      inline_str
          .replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, static_cast<char const*>("XXXZZZ"), 4);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXXXZg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and a count and character replaces
/// the characters in the range with the specified number of copies of the character
TEST(InlineStringReplace, ReplaceIteratorRangeCharAndCount) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 = inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, 5, 'Z');
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdZZZZZg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and an initializer list replaces
/// the characters in the range with those from the initializer list
TEST(InlineStringReplace, ReplaceIteratorRangeInitializerList) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  auto& inline_str_2 = inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, {'X', 'Y', 'Z'});
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXYZg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and another iterator range denoted
/// by forward iterators replaces the characters in the range with those from the other range
TEST(InlineStringReplace, ReplaceIteratorRangeWithIteratorRange) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;

  using forward_it = testing::demoted_iterator<arene::base::array<char, 3>::const_iterator, std::forward_iterator_tag>;
  arene::base::array<char, 3> const source{'X', 'Y', 'Z'};
  forward_it const forward_begin{source.begin()};
  forward_it const forward_end{source.end()};

  auto& inline_str_2 =
      inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, forward_begin, forward_end);
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXYZg");
}

/// @test Invoking `replace` with an iterator range referring to the `inline_string` and another iterator range denoted
/// by bidirectional iterators replaces the characters in the range with those from the other range
TEST(InlineStringReplace, ReplaceIteratorRangeWithRandomAccessIteratorRange) {
  inline_string<default_size> inline_str{"abcdefg"};
  auto const& const_inline_str = inline_str;
  arene::base::array<char, 3> const source{'X', 'Y', 'Z'};
  auto& inline_str_2 =
      inline_str.replace(const_inline_str.begin() + 4, const_inline_str.begin() + 6, source.begin(), source.end());
  ::testing::StaticAssertTypeEq<decltype(inline_str_2), decltype(inline_str)&>();
  ASSERT_EQ(inline_str, "abcdXYZg");
}

/// Constexpr wrapper around inline_string.replace for iterator ranges.
/// @return A copy of input string with the given iterator range replaced.
template <std::size_t MaxLength, typename... Args>
constexpr auto
do_iterator_replace(inline_string<MaxLength> inline_str, std::ptrdiff_t first, std::ptrdiff_t last, Args&&... args)
    -> inline_string<MaxLength> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  inline_str.replace(inline_str.begin() + first, inline_str.begin() + last, std::forward<Args>(args)...);
  return inline_str;
}

/// Constexpr wrapper around inline_string.copy, it produces str_from.copy(str_to.data(), Args...);
/// @return A pair that contains a copy of str_to post-copy and the number of characters copied.
template <typename... Args>
constexpr auto do_copy(inline_string<default_size> str_from, inline_string<default_size> str_to, Args... args)
    -> std::pair<inline_string<default_size>, std::size_t> {
  auto const num_chars_copied = str_from.copy(str_to.data(), std::forward<Args>(args)...);
  return {str_to, num_chars_copied};
}

/// @test Invoking `copy` copies the specified number of characters into the target buffer
TEST(InlineStringCopy, IfNLessThanOrEqualSizeCopiesNCharactersToDest) {
  ASSERT_EQ(do_copy("hello", "world", 0U).first, "world");
  ASSERT_EQ(do_copy("hello", "world", 0U).second, 0);
  ASSERT_EQ(do_copy("hello", "world", 3U).first, "helld");
  ASSERT_EQ(do_copy("hello", "world", 3U).second, 3);
  ASSERT_EQ(do_copy("hello", "world", 5U).first, "hello");
  ASSERT_EQ(do_copy("hello", "world", 5U).second, 5);
}

/// @test Invoking `copy` with a count greater than the length of the string copies the whole string into the target
/// buffer
TEST(InlineStringCopy, IfNGreaterSizeCopiesSizeCharactersToDest) {
  ASSERT_EQ(do_copy("hello", "world", 6U).first, "hello");
  ASSERT_EQ(do_copy("hello", "world", 6U).second, 5);
  ASSERT_EQ(do_copy("hello", "world", 100U).first, "hello");
  ASSERT_EQ(do_copy("hello", "world", 100U).second, 5);
}

/// @test Invoking `copy` with a position and count copies the specified number of characters from the specified
/// position into the target buffer
TEST(InlineStringCopy, IfPosLessThanSizeCopiesUpToNCharactersFromPosToDest) {
  ASSERT_EQ(do_copy("hello", "world", 0U, 0U).first, "world");
  ASSERT_EQ(do_copy("hello", "world", 0U, 0U).second, 0);
  ASSERT_EQ(do_copy("hello", "world", 3U, 0U).first, "helld");
  ASSERT_EQ(do_copy("hello", "world", 3U, 0U).second, 3);
  ASSERT_EQ(do_copy("hello", "world", 3U, 3U).first, "lorld");
  ASSERT_EQ(do_copy("hello", "world", 3U, 3U).second, 5 - 3);
}

/// @test Invoking `copy` with a position and count where the position is greater than the size of the string does
/// nothing
TEST(InlineStringCopy, IfPosGreaterSizeCopies0CharactersToDest) {
  ASSERT_EQ(do_copy("hello", "world", 6U, 6U).first, "world");
  ASSERT_EQ(do_copy("hello", "world", 6U, 6U).second, 0);
  ASSERT_EQ(do_copy("hello", "world", 6U, 100U).first, "world");
  ASSERT_EQ(do_copy("hello", "world", 6U, 100U).second, 0);
}

/// @test Invoking `substr` without any arguments returns a copy of the whole string
TEST(InlineStringSubstr, WithoutPosStartsSubstrAtBegin) {
  constexpr inline_string<default_size> inline_str{"abcdefgh"};
  ::testing::StaticAssertTypeEq<decltype(inline_str.substr()), arene::base::remove_cvref_t<decltype(inline_str)>>();
  ASSERT_EQ(inline_str.substr(), "abcdefgh");
}

/// @test Invoking `substr` with a position returns a new string holding the characters of the source string starting at
/// the specified position
TEST(InlineStringSubstr, WithPosStartsSubstrAtPos) {
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<inline_string<default_size>>().substr(5)), inline_string<default_size>>(
      );
  ASSERT_EQ(inline_string<default_size>{"abcdefgh"}.substr(3), "defgh");
}

/// @test Invoking `substr` with a position and a count returns a new string holding the characters of the source string
/// starting at the specified position, up to the specified number of characters
TEST(InlineStringSubstr, WithPosAndCountStartsSubstrAtPosExtendsCountClampedToSize) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>>().substr(5, 2)),
      inline_string<default_size>>();
  ASSERT_TRUE(inline_string<default_size>{"abcdefgh"}.substr(0, 0).empty());
  ASSERT_TRUE(inline_string<default_size>{"abcdefgh"}.substr(3, 0).empty());
  ASSERT_EQ(inline_string<default_size>{"abcdefgh"}.substr(3, 4), "defg");
  ASSERT_EQ(inline_string<default_size>{"abcdefgh"}.substr(3, 1000), "defgh");
}

/// @test Invoking `substr` with a position greater than the length of the string returns an empty string
TEST(InlineStringSubstr, IfPosGESizeIsEmptyString) {
  constexpr inline_string<default_size> inline_str{"abcdefgh"};
  ::testing::StaticAssertTypeEq<decltype(inline_str.substr(5)), arene::base::remove_cvref_t<decltype(inline_str)>>();
  ASSERT_TRUE(inline_str.substr(inline_str.size()).empty());
  ASSERT_TRUE(inline_str.substr(inline_str.size() + 100).empty());
  ASSERT_TRUE(inline_str.substr(inline_str.size(), 0).empty());
  ASSERT_TRUE(inline_str.substr(inline_str.size() + 100, 0).empty());
  ASSERT_TRUE(inline_str.substr(inline_str.size(), 10).empty());
  ASSERT_TRUE(inline_str.substr(inline_str.size() + 100, 10).empty());
}

/// @test Invoking `find` with the whole string returns zero
TEST(InlineStringFind, FindSelf) {
  constexpr inline_string<default_size> inline_str{"hello world"};
  ASSERT_EQ(inline_str.find(inline_str), 0);
}

/// @test Invoking `find` with a string not contained returns `npos`
TEST(InlineStringFind, UnmatchingStringNotFound) {
  constexpr inline_string<default_size> inline_str{"hello world"};
  constexpr inline_string<default_size> inline_str_2{"goodbye"};
  ASSERT_EQ(inline_str.find(inline_str_2), inline_string<default_size>::npos);
}

/// @test Invoking `find` with a string that is a substring returns the first index of that substring
TEST(InlineStringFind, SubstringFound) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find(inline_string<default_size>{"wor"}), 6);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find(string_view{}), 0);
  ASSERT_EQ(inline_string<default_size>{}.find(string_view{}), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find("o"_asv), 4);
}

/// @test Invoking `find` with a string that is a substring and a position returns the first index of that substring
/// after the specified position
TEST(InlineStringFind, FindWithPosition) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find("o"_asv, 5), 7);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find("hello"_asv, 1), inline_string<default_size>::npos);
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.find("hello"_asv, inline_string<default_size>::npos),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find` with a single character returns the first index of the character in the string
TEST(InlineStringFind, FindSingleChar) {
  ASSERT_EQ(inline_string<default_size>{"test"}.find('t'), 0);
  ASSERT_EQ(inline_string<default_size>{"test"}.find('s'), 2);
}

/// @test Invoking `find` with a single character and a position returns the first index of the character in the string
/// after the specified position
TEST(InlineStringFind, FindSingleCharWithStartPosition) {
  ASSERT_EQ(inline_string<default_size>{"test"}.find('t', 1), 3);
  ASSERT_EQ(inline_string<default_size>{"test"}.find('e', 2), inline_string<default_size>::npos);
}

/// @test Invoking `find` with a pointer to a NUL-terminated string returns the first index of the characters in the
/// NUL-terminated string in the string
TEST(InlineStringFind, SubstringFoundWithCharPointer) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find(inline_string<default_size>{"wor"}.c_str()), 6);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find(static_cast<char const*>("")), 0);
  ASSERT_EQ(string_view{}.find(static_cast<char const*>("")), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find(static_cast<char const*>("o")), 4);
}

/// @test Invoking `find` with a character pointer and a length returns the first index of the string formed from the
/// specified number of characters at the specified pointer
TEST(InlineStringFind, FindWithPositionWithCharPointer) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.find(static_cast<char const*>("o"), 5), 7);
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.find(static_cast<char const*>("hello"), 1),
      inline_string<default_size>::npos
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .find(static_cast<char const*>("hello"), inline_string<default_size>::npos),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find` with a character and a position and count returns the first index from the specified position
/// of the string of the specified length from the character pointer
TEST(InlineStringFind, FindWithPositionAndCount) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .find(inline_string<default_size>{"hello world"}.substr(4).c_str(), 5, 1),
      7
  );
  ASSERT_EQ(inline_string<default_size>{"hello hell"}.find(inline_string<default_size>{"hello hell"}.c_str(), 1, 4), 6);
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.find("hello", inline_string<default_size>::npos, 3),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `rfind` with the whole string returns zero
TEST(InlineStringRFind, ReverseFindSelf) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{"hello world"}), 0);
  ASSERT_EQ(inline_string<default_size>{}.rfind(inline_string<default_size>{}), 0);
}

/// @test Invoking `rfind` with a string that is not present returns `npos`
TEST(InlineStringRFind, UnmatchingStringNotFoundInReverse) {
  ASSERT_EQ(
      inline_string<default_size>{"hello"}.rfind(inline_string<default_size>{"goodbye"}),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `rfind` with a substring returns the index of that substring
TEST(InlineStringRFind, SubstringFoundInReverse) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{"wor"}), 6);
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{}),
      inline_string<default_size>{"hello world"}.length()
  );
  ASSERT_EQ(inline_string<default_size>{}.rfind(string_view{}), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("o"_asv), 7);
}

/// @test Invoking `rfind` with a substring and a position returns the index of that substring that is less than the
/// specified position
TEST(InlineStringRFind, ReverseFindWithPosition) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("o"_asv, 5), 4);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("world"_asv, 5), inline_string<default_size>::npos);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("hello"_asv, inline_string<default_size>::npos), 0);
}

/// @test Invoking `rfind` with a single character returns the index of the last instance of that character
TEST(InlineStringRFind, ReverseFindSingleChar) {
  ASSERT_EQ(inline_string<default_size>{"test"}.rfind('t'), 3);
  ASSERT_EQ(inline_string<default_size>{"test"}.rfind('s'), 2);
}

/// @test Invoking `rfind` with a single character and a position returns the index of the last instance of that
/// character less than the specified position
TEST(InlineStringRFind, ReverseFindSingleCharWithStartPosition) {
  ASSERT_EQ(inline_string<default_size>{"test"}.rfind('t', 1), 0);
  ASSERT_EQ(inline_string<default_size>{"test"}.rfind('s', 1), inline_string<default_size>::npos);
}

/// @test Invoking `rfind` with a pointer to a NUL-terminated string returns the index of the last instance of the
/// pointed-to string
TEST(InlineStringRFind, SubstringFoundWithCharPointerInReverse) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{"wor"}.c_str()), 6);
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("")),
      inline_string<default_size>{"hello world"}.length()
  );
  ASSERT_EQ(string_view{}.rfind(static_cast<char const*>("")), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("o")), 7);
}

/// @test Invoking `rfind` with a pointer to a NUL-terminated string and a position returns the index of the last
/// instance of the pointed-to string less than the specified position
TEST(InlineStringRFind, ReverseFindWithPositionWithCharPointer) {
  ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("o"), 5), 4);
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("world"), 1),
      inline_string<default_size>::npos
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .rfind(static_cast<char const*>("hello"), inline_string<default_size>::npos),
      0
  );
}

/// @test Invoking `rfind` with a character pointer and a position and length returns the index of the last
/// instance of the string consisting of the specified number of characters from the pointer that is less than the
/// position
TEST(InlineStringRFind, ReverseFindWithPositionAndCount) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .rfind(inline_string<default_size>{"hello world"}.substr(4).c_str(), 5, 1),
      4
  );
  ASSERT_EQ(
      inline_string<default_size>{"hell hello"}.rfind(inline_string<default_size>{"hell hello"}.c_str(), 7, 4),
      5
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind("good", inline_string<default_size>::npos, 3),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a list of characters that aren't present returns `npos`
TEST(InlineStringFindFirstOf, FindFirstOfNonMatchingList) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abc"}),
      inline_string<default_size>::npos
  );
  ASSERT_EQ(inline_string<default_size>{"banana"}.find_first_of("fruit"), inline_string<default_size>::npos);
  ASSERT_EQ(inline_string<default_size>{"banana"}.find_first_of(""), inline_string<default_size>::npos);
}

/// @test Invoking `find_first_of` with a list of characters where at least one is present returns the index of the
/// first character in the string that is in the list
TEST(InlineStringFindFirstOf, FindFirstOfMatchingList) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abcdef"}), 1);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"hw"}), 0);
  ASSERT_EQ(inline_string<default_size>{"woven planet"}.find_first_of(inline_string<default_size>{"abcdef"}), 3);
}

/// @test Invoking `find_first_of` with a list of characters and an offset where at least one is present returns the
/// index of the first character in the string that is in the list after the specified offset
TEST(InlineStringFindFirstOf, FindFirstOfWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("abcde"_asv, 2), 10);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("hw"_asv, 1), 6);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("fg%"_asv, 2), inline_string<default_size>::npos);
}

/// @test Invoking `find_first_of` with a list of characters specified by a NUL-terminated string where at least one is
/// present returns the index of the first character in the string that is in the list
TEST(InlineStringFindFirstOf, FindFirstOfMatchingListWithCharPointer) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abcdef"}.c_str()),
      1
  );
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"hw"}.c_str()), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("lw"), 2);
}

/// @test Invoking `find_first_of` with a list of characters specified by a NUL-terminated string and an offset where at
/// least one is present returns the index of the first character in the string that is in the list after the specified
/// offset
TEST(InlineStringFindFirstOf, FindFirstOfWithOffsetWithCharPointer) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(static_cast<char const*>("abcde"), 2), 10);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(static_cast<char const*>("hw"), 1), 6);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(static_cast<char const*>("fg%"), 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a single character returns the index of that character, or `npos` if it is not
/// found
TEST(InlineStringFindFirstOf, FindFirstOfWithCharacter) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('e'), 1);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('h'), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('a'), inline_string<default_size>::npos);
}

/// @test Invoking `find_first_of` with a single character and an offset returns the index of that character after the
/// offset, or `npos` if it is not found
TEST(InlineStringFindFirstOf, FindFirstOfCharacterWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('l', 5), 9);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('h', 1), inline_string<default_size>::npos);
}

/// @test Invoking `find_first_of` with a list of characters specified by a string literal and an offset and length
/// where at least one is present returns the index of the first character in the string that is in the list specified
/// as the string of the specified length from the character pointer, after the specified offset in the searched string
TEST(InlineStringFindFirstOf, FindFirstOfMatchingListWithOffsetAndLength) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("abcdef", 0, 4), 10);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of("helow", 2, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a list of characters specified by a character pointer and an offset and length
/// where at least one is present returns the index of the first character in the string that is in the list specified
/// as the string of the specified length from the character pointer, after the specified offset in the searched string
TEST(InlineStringFindFirstOf, FindFirstOfMatchingListWithCharPointerAndLength) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abcdef"}.c_str(), 0, 4),
      10
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"hw"}.c_str(), 2, 1),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_not_of` with a list of characters returns the index of the first character not in the
/// list, or `npos` if all the characters in the string are in the list.
TEST(InlineStringFindFirstNotOf, FindFirstNotOf) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of("abc"_asv), 0);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(inline_string<default_size>{"hello world!"}),
      inline_string<default_size>::npos
  );
  ASSERT_EQ(inline_string<default_size>{"woven planet"}.find_first_not_of(string_view{}), 0);
  ASSERT_EQ(inline_string<default_size>{"woven planet"}.find_first_not_of("avowed"), 4);
}

/// @test Invoking `find_first_not_of` with a list of characters and an offset returns the index of the first character
/// not in the list after the offset, or `npos` if all the characters in the string after the offset are in the list.
TEST(InlineStringFindFirstNotOf, FindFirstNotOfWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of("abcde"_asv, 2), 2);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of(" elo"_asv, 1), 6);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of("world! "_asv, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a NUL-terminated string returns the index
/// of the first character not in the list, or `npos` if all the characters in the string are in the list.
TEST(InlineStringFindFirstNotOf, FindFirstNotOfMatchingListWithCharPointer) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(inline_string<default_size>{"abcdef"}.c_str()),
      0
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(inline_string<default_size>{"hello world!"}.c_str()
      ),
      inline_string<default_size>::npos
  );
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of("helo"), 5);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a NUL-terminated string and an offset
/// returns the index of the first character not in the list after the offset, or `npos` if all the characters in the
/// string after the offset are in the list.
TEST(InlineStringFindFirstNotOf, FindFirstNotOfWithOffsetWithCharPointer) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of(static_cast<char const*>("abcde"), 2), 2);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of(static_cast<char const*>("elo "), 1), 6);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(static_cast<char const*>("world! %^zz"), 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_not_of` with a single character returns the index of the first character
/// not equal to that character, or `npos` if all the characters in the string are the same as the character
TEST(InlineStringFindFirstNotOf, FindFirstNotOfWithCharacter) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('e'), 0);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('h'), 1);
  ASSERT_EQ(inline_string<default_size>("a").find_first_not_of('a'), inline_string<default_size>::npos);
  ASSERT_EQ(inline_string<default_size>{"aaaaab"}.find_first_not_of('a'), 5);
}

/// @test Invoking `find_first_not_of` with a single character and an offset returns the index of the first character
/// not equal to the character after the offset
TEST(InlineStringFindFirstNotOf, FindFirstNotOfCharacterWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('h', 5), 5);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('l', 2), 4);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a character pointer and an offset and
/// length returns the index of the first character not in the string of characters of the specified length from the
/// pointer after the offset, or `npos` if all the characters in the string after the offset are in the list.
TEST(InlineStringFindFirstNotOf, FindFirstNotOfMatchingListWithCharPointerAndLength) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}
          .find_first_not_of(inline_string<default_size>{"abcdef"}.c_str(), 1, 4),
      1
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}
          .find_first_not_of(inline_string<default_size>{"eorld! w"}.c_str(), 1, 1),
      2
  );
  ASSERT_EQ(inline_string<default_size>{"xxcccdddyyy"}.find_first_not_of("abcdef", 2, 3), 5);
}

/// @test Invoking `find_last_of` with a list of characters returns the index of the last character in the string that
/// is not in the list, or `npos` if none of the characters are in the list
TEST(InlineStringFindLastOf, FindLastOf) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv), 15);
  ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(inline_string<default_size>{"goodbye everybody!"}),
      inline_string<default_size>{"goodbye everybody!"}.length() - 1
  );
  ASSERT_EQ(inline_string<default_size>{"xxAxxxA"}.find_last_of("yzx"_asv), 5);
  ASSERT_EQ(inline_string<default_size>{"xxAxxxA"}.find_last_of("abxd"), 5);
  ASSERT_EQ(inline_string<default_size>{"xxAxxxA"}.find_last_of(""_asv), inline_string<default_size>::npos);
}

/// @test Invoking `find_last_of` with a list of characters and an offset returns the index of the last character in the
/// string before the offset that is not in the list, or `npos` if none of the characters before the offset are in the
/// list
TEST(InlineStringFindLastOf, FindLastOfWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv, 8), 4);
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv, 3), 3);
  ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a list of characters specified by a NUL-terminated string returns the index of
/// the last character in the string that is not in the list, or `npos` if none of the characters are in the list
TEST(InlineStringFindLastOf, FindLastOfWithCharPtrConversion) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd")), 15);
  ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(inline_string<default_size>{"goodbye everybody!"}),
      inline_string<default_size>{"goodbye everybody!"}.length() - 1
  );
}

/// @test Invoking `find_last_of` with a list of characters specified by a NUL-terminated string and an offset returns
/// the index of the last character in the string before the offset that is not in the list, or `npos` if none of the
/// characters before the offset are in the list
TEST(InlineStringFindLastOf, FindLastOfWithOffsetWithCharPtrConversion) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd"), 8), 4);
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd"), 3), 3);
  ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd"), 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a list of characters specified by a character pointer and an offset and a count
/// returns the index of the last character in the string before the offset that is not in a list made of the first N
/// characters from the character pointer, or `npos` if none of the characters before the offset are in the list
TEST(InlineStringFindLastOf, FindLastOfWithOffsetAndCountWithCharPtrConversion) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcde"), 8, 4), 4);
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("gxye", 8, 2), 0);
  ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcdefg"), 2, 4),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a single character returns the last index of that character, or `npos` if it is
/// not found
TEST(InlineStringFindLastOf, FindLastOfChar) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of('d'), 15);
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of('x'), inline_string<default_size>::npos);
}

/// @test Invoking `find_last_of` with a single character and an offset returns the index of that character before the
/// offset, or `npos` if it is not found
TEST(InlineStringFindLastOf, FindLastOfCharWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of('d', 8), 3);
  ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of('y', 2), inline_string<default_size>::npos);
}

/// @test Invoking `find_last_not_of` with a list of characters returns the index of the last character not in the
/// list, or `npos` if all the characters in the string are in the list.
TEST(InlineStringFindLastNotOf, FindLastNotOf) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of("abc"_asv),
      inline_string<default_size>{"hello world!"}.length() - 1
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"hello world!"}),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters and an offset returns the index of the last character
/// not in the list before the offset, or `npos` if all the characters in the string before the offset are in the list.
TEST(InlineStringFindLastNotOf, FindLastNotOfWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of("abcde"_asv, 10), 9);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of("elowr"_asv, 9), 5);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of("whelo "_asv, 7),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a NUL-terminated string returns the index
/// of the last character not in the list, or `npos` if all the characters in the string are in the list.
TEST(InlineStringFindLastNotOf, FindLastNotOfMatchingListWithCharPointer) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"abcdef"}.c_str()),
      inline_string<default_size>{"hello world!"}.length() - 1
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"hello world!"}.c_str()),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a NUL-terminated string and an offset
/// returns the index of the last character not in the list before the offset, or `npos` if all the characters in the
/// string before the offset are in the list.
TEST(InlineStringFindLastNotOf, FindLastNotOfWithOffsetWithCharPointer) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of(static_cast<char const*>("abcde"), 10), 9);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of(static_cast<char const*>("elowr"), 9), 5);
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(static_cast<char const*>("whelo "), 7),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a single character returns the index of the last character
/// not equal to that character, or `npos` if all the characters in the string are the same as the character
TEST(InlineStringFindLastNotOf, FindLastNotOfWithCharacter) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of('e'),
      inline_string<default_size>{"hello world!"}.length() - 1
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of('!'),
      inline_string<default_size>{"hello world!"}.length() - 2
  );
  ASSERT_EQ(inline_string<default_size>("a").find_last_not_of('a'), inline_string<default_size>::npos);
}

/// @test Invoking `find_last_not_of` with a single character and an offset returns the index of the last character
/// not equal to the character before the offset
TEST(InlineStringFindLastNotOf, FindLastNotOfCharacterWithOffset) {
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of('h', 5), 5);
  ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of('l', 3), 1);
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a character pointer and an offset and
/// length returns the index of the last character not in the string of characters of the specified length from the
/// pointer before the offset, or `npos` if all the characters in the string before the offset are in the list.
TEST(InlineStringFindLastNotOf, FindLastNotOfMatchingListWithCharPointerAndLength) {
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"abcdef"}.c_str(), 1, 4),
      1
  );
  ASSERT_EQ(
      inline_string<default_size>{"hello world!"}
          .find_last_not_of(inline_string<default_size>{"eorld! w"}.c_str(), 1, 1),
      0
  );
  ASSERT_EQ(inline_string<default_size>{"aaaaayyyxxaaa"}.find_last_not_of("abcx", 11, 3), 9);
}

/// @test `starts_with` returns `true` if and only if the string starts with the specified characters
TEST(InlineStringStartsWith, StartsWith) {
  ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"hello"}));
  ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"hell"}));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"goodbye"}));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with(string_view{"goodbye"}));
  ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"hell"}.c_str()));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with("hello world"));

  ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with('h'));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with('x'));
  ASSERT_FALSE(inline_string<default_size>{}.starts_with('x'));

  ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().starts_with('h')));
  ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().starts_with("h")));
  ASSERT_TRUE(
      noexcept(std::declval<inline_string<default_size>>().starts_with(std::declval<inline_string<default_size>>()))
  );
}

/// @test `ends_with` returns `true` if and only if the string ends with the specified characters
TEST(InlineStringEndsWith, EndsWith) {
  ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"hello"}));
  ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"ello"}));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"goodbye"}));
  ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(string_view{"ello"}));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.ends_with(string_view{"goodbye"}));
  ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"ello"}.c_str()));
  ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with("ello"));
  ASSERT_FALSE(inline_string<default_size>{}.ends_with("goodbye"));

  ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with('o'));
  ASSERT_FALSE(inline_string<default_size>{"hello"}.ends_with('x'));
  ASSERT_FALSE(inline_string<default_size>{}.ends_with('x'));

  ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().ends_with('h')));
  ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().ends_with("h")));
  ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().ends_with(std::declval<inline_string<default_size>>()
  )));
}

/// @test Concatenating strings with `+` yields a string with the appropriate contents
TEST(InlineStringOperatorPlus, WithInlineStringsCreatesNewStringThatIsConcatinationOfInputStrings) {
  ASSERT_EQ(inline_string<default_size>{"hello"} + inline_string<default_size>{" world"}, "hello world");
}

/// @test Concatenating strings with `+` yields a string with the sum of the capacities
TEST(InlineStringOperatorPlus, WithInlineStringsReturnedStringCapacityIsSumOfCapacities) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>>() + std::declval<inline_string<default_size>>()),
      inline_string<default_size + default_size>>();
}

/// @test Concatenating an `inline_string` and a `string_view` with `+` yields a string with the appropriate contents
TEST(InlineStringOperatorPlus, WithStringViewCreatesNewStringThatIsConcatinationOfInputStrings) {
  ASSERT_EQ(inline_string<default_size>{"hello"} + string_view{" world"}, "hello world");
  ASSERT_EQ(string_view{"hello"} + inline_string<default_size>{" world"}, "hello world");
}

/// @test Concatenating an `inline_string` and a `string_view` with `+` yields a string with the capacity of the
/// supplied `inline_string`
TEST(InlineStringOperatorPlus, WithStringViewReturnedStringCapacityIsUnchanged) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>>() + std::declval<string_view>()),
      inline_string<default_size>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<string_view>() + std::declval<inline_string<default_size>>()),
      inline_string<default_size>>();
}

/// @test `compare` returns greater than zero when comparing a non-empty string against an empty string
TEST(InlineStringCompare, AgainstEmptyIsGreater) {
  ASSERT_GT(inline_string<default_size>{"hello"}.compare(string_view{""}), 0);
}

/// @test `compare` returns less than zero when comparing an empty string against a non-empty string
TEST(InlineStringCompare, AsEmptyToNonEmptyIsLess) {
  ASSERT_LT(inline_string<default_size>{""}.compare(string_view{"hello"}), 0);
}

/// @test `compare` returns zero when comparing two empty strings
TEST(InlineStringCompare, AsEmptyToEmptyIsEqual) {
  ASSERT_EQ(inline_string<default_size>{""}.compare(string_view{""}), 0);
}

/// @test `compare` returns zero when comparing a string against itself
TEST(InlineStringCompare, AgainstSelfIsEqual) {
  ASSERT_EQ(inline_string<default_size>{"hello"}.compare(string_view{"hello"}), 0);
}

/// @test `compare` returns greater than zero when comparing a string against another which is a prefix
TEST(InlineStringCompare, AgainstStrictPrefixIsGreater) {
  ASSERT_GT(inline_string<default_size>{"hello"}.compare(string_view{"hell"}), 0);
}

/// @test `compare` returns less than zero when comparing a string against another which it is a prefix of
TEST(InlineStringCompare, AgainstAsStrictPrefixIsLess) {
  ASSERT_LT(inline_string<default_size>{"hell"}.compare(string_view{"hello"}), 0);
}

/// @test `compare` returns greater than zero if the string is lexicographically after the other string
TEST(InlineStringCompare, AgainstLexicographicallyLessIsGreater) {
  ASSERT_GT(inline_string<default_size>{"b"}.compare(string_view{"a"}), 0);
  ASSERT_GT(inline_string<default_size>{"bat"}.compare(string_view{"bac"}), 0);
  ASSERT_GT(inline_string<default_size>{"bat"}.compare(string_view{"bacz"}), 0);
  ASSERT_GT(inline_string<default_size>{"bata"}.compare(string_view{"bac"}), 0);
}

/// @test `compare` returns less than zero if the string is lexicographically before the other string
TEST(InlineStringCompare, AgainstLexicographicallyGreaterIsLess) {
  ASSERT_LT(inline_string<default_size>{"a"}.compare(string_view{"b"}), 0);
  ASSERT_LT(inline_string<default_size>{"bac"}.compare(string_view{"bat"}), 0);
  ASSERT_LT(inline_string<default_size>{"bacz"}.compare(string_view{"bat"}), 0);
  ASSERT_LT(inline_string<default_size>{"bac"}.compare(string_view{"bata"}), 0);
}

/// @test Invoking `compare` with an offset and size is equivalent to comparing the appropriate substring
TEST(InlineStringCompare, WithOffsetAndSizeIsEquivalentToCompareAgainstSubstrWithOffsetAndSize) {
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, string_view{"b"}),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare(string_view{"b"})
  );
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, string_view{"b"}),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare(string_view{"b"})
  );
  ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, string_view{"bcd"}),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare(string_view{"bcd"})
  );
}

/// @test Invoking `compare` with an offset and size and `string_view` and position is equivalent to comparing the
/// appropriate substring against the substring of the view from the position
TEST(
    InlineStringCompare,
    WithOffsetAndSizePlusOffsetIsEquivalentToCompareAgainstSubstrWithOffsetAndSizeAndSubstrWithOffset
) {
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, string_view{"b"}, 0U),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare(string_view{"b"}.substr(0U))
  );
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, string_view{"b"}, 1U),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare(string_view{"b"}.substr(1U))
  );
  ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, string_view{"bcd"}, 1U),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare(string_view{"bcd"}.substr(1U))
  );
}

/// @test Invoking `compare` with an offset and size and `string_view` and position and length is equivalent to
/// comparing the appropriate substring against the substring of the view from the position with the specified length
TEST(
    InlineStringCompare,
    WithOffsetAndSizePlusOffsetAndSizeIsEquivalentToCompareAgainstSubstrWithOffsetAndSizeAndSubstrWithOffsetAndSize
) {
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, string_view{"b"}, 0U, 1U),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare(string_view{"b"}.substr(0U, 1U))
  );
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, string_view{"b"}, 1U, 1U),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare(string_view{"b"}.substr(1U, 1U))
  );
  ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, string_view{"bcd"}, 1U, 10U),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare(string_view{"bcd"}.substr(1U, 10U))
  );
}

/// @test Invoking `compare` with an offset and size and character pointer and length is equivalent to comparing the
/// appropriate substring against the substring of the character pointer with the length
TEST(
    InlineStringCompare,
    WithOffsetAndSizePlusCStrAndSizeIsEquivalentToCompareAgainstSubstrWithOffsetAndSizeAndCStrWithSize
) {
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, "b", 1U),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare("b")
  );
  ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, "b", 0U),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare("")
  );
  ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, "bcd", 2U),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare("bc")
  );
}

/// @test `data` invoked on a `const` string returns a pointer to `const`
TEST(InlineString, CanGetDataOnConstObject) {
  constexpr inline_string<default_size> inline_str{"hello world"};
  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(inline_str.data(), &inline_str[0]);
  ::testing::StaticAssertTypeEq<decltype(inline_str.data()), char const*>();
  ASSERT_TRUE(noexcept(inline_str.data()));
}

/// @test `data` invoked on a non-`const` string returns a pointer to non-`const`
TEST(InlineString, CanGetDataOnNonConstObject) {
  inline_string<default_size> inline_str{"hello world"};
  ASSERT_TRUE(noexcept(inline_str.data()));
  ::testing::StaticAssertTypeEq<decltype(inline_str.data()), char*>();
  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(inline_str.data(), &inline_str[0]);
  // NOLINTNEXTLINE(bugprone-assignment-in-if-condition) Outside the macro this is in an assertion, not an if condition
  ASSERT_EQ((*inline_string<default_size>{"hello"}.data() = 'a'), 'a');
}

/// @test An `inline_string` can be converted to a `null_terminated_string_view` with the pointer equal to that returned
/// from `c_str`
TEST(InlineString, CanConvertToNullTerminatedString) {
  constexpr auto* original = "hello world";
  ASSERT_STREQ(
      null_terminated_string_view{inline_string<default_size>{original}}.c_str(),
      null_terminated_string_view{original}.c_str()
  );
}

// The following functions and the InlineStringForBugProneNoexceptClangTidy tests would have triggered a clang-tidy
// warning about bugprone-noexcept prior to the commit where they were introduced.
namespace noexcept_correctness_check {
constexpr std::size_t size = 123;

/// @brief Copy inline string
/// @param str original value
/// @return Copied value
auto copy(inline_string<size> const& str) noexcept -> inline_string<size> { return str; }

/// @brief Move inline string
/// @param str original value
// NOLINTBEGIN(hicpp-move-const-arg, misc-const-correctness) This is explciitly testing the move behavior
void move(inline_string<size>&& str) noexcept {
  inline_string<size> local(std::move(str));
  str = std::move(local);
}

template <std::size_t SourceSize, std::size_t TargetSize>
void do_string_assign(inline_string<SourceSize> const& source, inline_string<TargetSize>& target) noexcept {
  target = source;
}

}  // namespace noexcept_correctness_check

/// @test Copy and move are `noexcept`
TEST(InlineStringForBugProneNoexceptClangTidy, NoexceptCopyAndMove) {
  inline_string<noexcept_correctness_check::size> dummy;
  dummy = noexcept_correctness_check::copy(dummy);
  noexcept_correctness_check::move(std::move(dummy));
}

// NOLINTEND(hicpp-move-const-arg, misc-const-correctness) This is explciitly testing the move behavior

/// @test Assignment from a smaller capacity string is `noexcept`
TEST(InlineStringAssignment, AssignmentFromSmallerSizeIsNoexcept) {
  inline_string<default_size> const source{"hello"};
  inline_string<default_size + 1> target;

  static_assert(noexcept(target = source), "Must be noexcept");
  noexcept_correctness_check::do_string_assign(source, target);
  ASSERT_EQ(source, target);
}

/// @test Assignment from a larger capacity string is `noexcept`
TEST(InlineStringAssignment, AssignmentFromLargerSizeIsNoexcept) {
  inline_string<default_size + 1> const source{"hello"};
  inline_string<default_size> target;

  static_assert(noexcept(target = source), "Must be noexcept");
  target = source;
  ASSERT_EQ(source, target);
}

/// @test `try_construct` returns an empty optional if the size of the string is larger than the capacity
TEST(InlineStringTryConstruct, FromInlineStringReturnsNulloptIfSizeGreaterThanCapacity) {
  inline_string<default_size + 1> too_large;
  too_large.resize(decltype(too_large)::max_size(), 'f');
  EXPECT_EQ(inline_string<default_size>::try_construct(too_large), nullopt);
}

/// @test `try_construct` returns an optional holding the desired string if the size fits in the capacity
TEST(InlineStringTryConstruct, FromInlineStringReturnsStringIfSizeLessThanOrEqualCapacity) {
  {
    SCOPED_TRACE("Input with > capacity");
    ASSERT_EQ(
        (inline_string<default_size>::try_construct(inline_string<default_size + 1>{"abc"})),
        inline_string<default_size>{"abc"}
    );
    inline_string<default_size + 1> exaclty_capacity;
    exaclty_capacity.resize(default_size, 'f');
    EXPECT_EQ((inline_string<default_size>::try_construct(exaclty_capacity)), exaclty_capacity);
  }
  {
    SCOPED_TRACE("Input with <= capacity");
    ASSERT_EQ(
        (inline_string<default_size>::try_construct(inline_string<default_size>{"abc"})),
        inline_string<default_size>{"abc"}
    );
    inline_string<default_size> exaclty_capacity;
    exaclty_capacity.resize(default_size, 'f');
    EXPECT_EQ((inline_string<default_size>::try_construct(exaclty_capacity)), exaclty_capacity);
  }
}

/// @test `try_construct` returns an empty optional if passed a `string_view` with a size larger than the capacity
TEST(InlineStringTryConstruct, FromStringViewReturnsNulloptIfSizeGreaterThanCapacity) {
  EXPECT_EQ(inline_string<default_size>::try_construct(too_long_view), nullopt);
}

/// @test `try_construct` returns an optional holding the desired string if the size fits in the capacity when
/// passed a `string_view`
TEST(InlineStringTryConstruct, FromStringViewReturnsStringIfSizeLessThanOrEqalCapacity) {
  ASSERT_EQ((inline_string<default_size>::try_construct(string_view{"abc"})), inline_string<default_size>{"abc"});
  EXPECT_EQ(
      inline_string<default_size>::try_construct(exact_length_view),
      inline_string<default_size>{exact_length_view}
  );
}

/// @test `try_construct` returns an empty optional if passed an iterator range with a size larger than the capacity
TEST(InlineStringTryConstruct, FromIteratorsReturnsNulloptIfDistanceGreaterThanCapacity) {
  SCOPED_TRACE("Random access");
  EXPECT_EQ(inline_string<default_size>::try_construct(too_long_view.begin(), too_long_view.end()), nullopt);
}

/// @test `try_construct` returns an optional holding the desired string if the size fits in the capacity when
/// passed an iterator range
TEST(InlineStringTryConstruct, FromIteratorsReturnsStringIfSizeLessThanOrEqalCapacity) {
  EXPECT_EQ(
      inline_string<default_size>::try_construct(exact_length_view.begin(), exact_length_view.end()),
      (inline_string<default_size>{exact_length_view.begin(), exact_length_view.end()})
  );

  constexpr string_view smaller_capacity{"abc"};
  EXPECT_EQ(
      inline_string<default_size>::try_construct(smaller_capacity.begin(), smaller_capacity.end()),
      (inline_string<default_size>{smaller_capacity.begin(), smaller_capacity.end()})
  );
}

template <typename InlineStringT>
using use_at = decltype(std::declval<InlineStringT>().at(0));

template <typename InlineStringT>
constexpr bool has_at_v = arene::base::substitution_succeeds<use_at, InlineStringT>;

/// @test Given an instance of `inline_string`, then `inline_string::at()`'s invocability matches
/// `arene::base::are_exceptions_enabled_v`.
TEST(InlineStringAt, AtInvocabilityMatchesAreExceptionsEnabled) {
  STATIC_ASSERT_EQ(has_at_v<inline_string<default_size>&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<inline_string<default_size> const&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<inline_string<default_size>&&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<inline_string<default_size> const&&>, arene::base::detail::are_exceptions_enabled_v);
}

/// @test The `is_inline_buffer_string` type trait returns true if the given type is an `inline_string` and false
/// otherwise.
TEST(InlineStringTypeTrait, IsInlineBufferStringChecksForAnInlineString) {
  STATIC_ASSERT_TRUE(arene::base::is_inline_buffer_string_v<inline_string<default_size>>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_buffer_string_v<inline_string<0>>);

  STATIC_ASSERT_FALSE(arene::base::is_inline_buffer_string_v<string_view>);
  STATIC_ASSERT_FALSE(arene::base::is_inline_buffer_string_v<char*>);
  STATIC_ASSERT_FALSE(arene::base::is_inline_buffer_string_v<int>);
}

/// @test The `is_inline_string` type trait returns true if the given type is an
/// `inline_string` and false otherwise.
TEST(InlineStringTypeTrait, IsInlineStringChecksForAnInlineString) {
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<default_size>>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<0>>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<default_size> const>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<0> const>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<default_size> volatile>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<0> volatile>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<default_size> const volatile>);
  STATIC_ASSERT_TRUE(arene::base::is_inline_string_v<inline_string<0> const volatile>);

  STATIC_ASSERT_FALSE(arene::base::is_inline_string_v<string_view>);
  STATIC_ASSERT_FALSE(arene::base::is_inline_string_v<char*>);
  STATIC_ASSERT_FALSE(arene::base::is_inline_string_v<int>);
  STATIC_ASSERT_FALSE(arene::base::is_inline_string_v<inline_string<0>&>);
  STATIC_ASSERT_FALSE(arene::base::is_inline_string_v<inline_string<0>&&>);
}

/// @test `make_inline_string` can be used with a string literal and is equal to
/// an `inline_string` explicitly constructed from a string literal.
TEST(InlineStringTypeTrait, MakeInlineStringWithStringLiteral) {
  constexpr auto str1 = arene::base::make_inline_string("hello, world!");

  STATIC_ASSERT_EQ(str1.size(), arene::base::string_length("hello, world!"));
  STATIC_ASSERT_EQ(str1.capacity(), arene::base::string_length("hello, world!"));
  STATIC_ASSERT_EQ(str1, arene::base::inline_string<str1.size()>{"hello, world!"});

  constexpr auto str2 = arene::base::make_inline_string("");

  STATIC_ASSERT_EQ(str2.size(), arene::base::string_length(""));
  STATIC_ASSERT_EQ(str2.capacity(), arene::base::string_length(""));
  STATIC_ASSERT_EQ(str2, arene::base::inline_string<str2.size()>{""});

  // Without this non-constexpr usage, tooling reports missing line coverage.
  auto const str3 = arene::base::make_inline_string("hello, world!");
  ASSERT_EQ(str1, str3);
}

}  // namespace
