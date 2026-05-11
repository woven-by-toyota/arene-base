// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/strings/algorithm/length.hpp"
// IWYU pragma: no_include "arene/base/strings/detail/string_length.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace {

using ::arene::base::inline_string;
using ::arene::base::literals::operator""_asv;
using ::arene::base::array;
using ::arene::base::null_terminated_string_view;
using ::arene::base::string_view;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

/// @test A default-constructed `inline_string` is empty, with a size of 0, and `c_str` returns a pointer to a
/// zero-length NUL-terminated string
TEST(ConstexprInlineStringCtors, DefaultConstructedStringsAreBlank) {
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<inline_string<default_size>>::value);
}

/// @test If an `inline_string` is constructed from a string literal, then the length of the string is the same as that
/// of the string literal, and `c_str` returns a pointer to a NUL-terminated string with the same contents as the string
/// literal.
TEST(ConstexprInlineStringCtors, IfInitializedFromStringLiteralContentsAreTheSame) {
  inline_string<default_size> const literal_initialized("hello");

  ASSERT_TRUE(!literal_initialized.empty());
  ASSERT_EQ(literal_initialized.size(), arene::base::string_length("hello"));
  ASSERT_NE(literal_initialized.c_str(), nullptr);
  ASSERT_EQ(*literal_initialized.c_str(), 'h');
  ASSERT_STREQ(literal_initialized.c_str(), "hello");
}

/// @test If an `inline_string` is constructed from a given length and a character at compile time, the initialized
/// string is filled with the given character with the given length.
TEST(ConstexprInlineStringCtors, IfInitializedFromLengthAndCharacterAsPrefilledString) {
  static constexpr std::size_t len{6U};
  static constexpr char chr{'b'};
  static constexpr inline_string<default_size> string_initialized{len, chr};

  STATIC_ASSERT_FALSE(string_initialized.empty());
  STATIC_ASSERT_EQ(string_initialized.size(), len);
  STATIC_ASSERT_EQ(string_initialized, "bbbbbb");
  STATIC_ASSERT_NE(string_initialized.c_str(), nullptr);

  STATIC_ASSERT_EQ(string_initialized[0], 'b');
  STATIC_ASSERT_EQ(string_initialized[1], 'b');
  STATIC_ASSERT_EQ(string_initialized[2], 'b');
  STATIC_ASSERT_EQ(string_initialized[3], 'b');
  STATIC_ASSERT_EQ(string_initialized[4], 'b');
  STATIC_ASSERT_EQ(string_initialized[5], 'b');
  STATIC_ASSERT_EQ(string_initialized[len], '\0');
}

/// @test After copying an `inline_string` from another `inline_string` with the same capacity, an `inline_string` with
/// a different capacity, or a string literal, whether via copy-construction or assignment, the target `inline_string`
/// compares equal to the source. Where the source is an `inline_string`, invoking `c_str` on the source and destination
/// yields different pointers.
TEST(ConstexprInlineStringCtors, CopyingAndAssignment) {
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
TEST(ConstexprInlineStringComparisonOperator, StringComparisons) {
  constexpr auto buffer_size1 = 42;
  constexpr auto buffer_size2 = 5;
  constexpr inline_string<buffer_size1> str_1("hello");
  constexpr inline_string<buffer_size1> str_2_const("goodbye");
  constexpr inline_string<buffer_size2> str_3("hello");
  constexpr inline_string<buffer_size2> str_4_const("hell");
  constexpr inline_string<buffer_size2> str_5("world");

  STATIC_ASSERT_EQ(str_1, str_1);
  STATIC_ASSERT_EQ(str_1, str_3);
  STATIC_ASSERT_NE(str_1, str_5);
  STATIC_ASSERT_NE(str_1, str_2_const);
  STATIC_ASSERT_NE(str_2_const, str_1);
  STATIC_ASSERT_EQ(str_2_const, str_2_const);
  STATIC_ASSERT_NE(str_2_const, str_4_const);

  STATIC_ASSERT_LT(str_1, str_5);
  STATIC_ASSERT_LT(str_4_const, str_3);
  STATIC_ASSERT_LT(str_2_const, str_4_const);
  STATIC_ASSERT_LE(str_1, str_5);
  STATIC_ASSERT_LE(str_4_const, str_3);
  STATIC_ASSERT_LE(str_2_const, str_4_const);

  STATIC_ASSERT_GT(str_5, str_1);
  STATIC_ASSERT_GT(str_3, str_4_const);
  STATIC_ASSERT_GT(str_4_const, str_2_const);
  STATIC_ASSERT_GE(str_5, str_1);
  STATIC_ASSERT_GE(str_3, str_4_const);
  STATIC_ASSERT_GE(str_4_const, str_2_const);
}

/// @test The index operator on a `const` `inline_string` returns a `const` reference to the character
TEST(ConstexprInlineStringIndexOperator, ReturnsConstReferenceToCharIfConst) {
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<inline_string<default_size> const&>().operator[](0)), char const&>();
}

/// @test The index operator on a non-`const` `inline_string` returns a non-`const` reference to the character
TEST(ConstexprInlineStringIndexOperator, ReturnsReferenceToCharIfNonConst) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<inline_string<default_size>&>().operator[](0)), char&>();
}

/// @test The index operator returns the character with the appropriate index. If the index is equal to the size of the
/// string, the returned character is NUL.
TEST(ConstexprInlineStringIndexOperator, ReturnsCharacterAtIndexConst) {
  constexpr auto* expected_string = "hello";
  constexpr inline_string<default_size> str{expected_string};
  STATIC_ASSERT_EQ(str[0], 'h');
  STATIC_ASSERT_EQ(str[1], 'e');
  STATIC_ASSERT_EQ(str[str.size()], '\0');
}

/// @test `inline_string` iterators are random access
TEST(ConstexprInlineStringIterator, IsRandomAccessIterator) {
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<inline_string<default_size>::iterator>);
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<inline_string<default_size>::const_iterator>);
}

/// @test The iterator returned from `begin` on an `inline_string` can be indexed to access the characters the same as
/// using the index operator on the string itself.
TEST(ConstexprInlineStringIterator, RandomAccessFromIterator) {
  constexpr inline_string<default_size> str("hello");

  STATIC_ASSERT_EQ(str[0], str.begin()[0]);
  STATIC_ASSERT_EQ(str[1], str.begin()[1]);
  STATIC_ASSERT_EQ(str[2], str.begin()[2]);
  STATIC_ASSERT_EQ(str[3], str.begin()[3]);
  STATIC_ASSERT_EQ(str[4], str.begin()[4]);
}

/// @test Accessing elements via `inline_string` iterators is consistent with accessing them via the index operator on
/// the string itself, or via the pointer returned from `data`
TEST(ConstexprInlineStringIterator, ElementAccess) {
  constexpr inline_string<default_size> str("hello");

  STATIC_ASSERT_EQ(*str.begin(), str[0]);
  STATIC_ASSERT_EQ(str.begin().operator->(), str.data());
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
TEST(ConstexprInlineStringIterator, CanConvertAndCompareConstAndNonConstIterators) {
  constexpr itr_compare_and_convert results = do_constexpr_iterator_convert_and_compare("hello");
  STATIC_ASSERT_TRUE(results.compare_non_const_to_const);
  STATIC_ASSERT_TRUE(results.compare_const_to_non_const);
  STATIC_ASSERT_TRUE(results.convert_non_const_to_const);
}

/// @test `begin`, `end`, `cbegin` and `cend` return appropriate iterators. These iterators allow iteration over the
/// characters in the string.
TEST(ConstexprInlineStringIteration, CanIterateOverElements) {
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
}

/// @test The `reverse_iterator` and `const_reverse_iterator` instances returned from `rbegin` and `rend` or `crbegin`
/// and `crend` can be used to iterate through the characters in a string in reverse order.
TEST(ConstexprInlineStringIteration, ReverseIteration) {
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
TEST(ConstexprInlineStringAppend, StringLiteralAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", " world"), "hello world");
}

/// @test Invoking the `append` member function with a pointer to a NUL-terminated string increases the string length by
/// the length of the NUL-terminated string, and copies the characters from the NUL-terminated string onto the end of
/// the string.
TEST(ConstexprInlineStringAppend, CStringAtEndOfString) {
  constexpr auto* world = " world";
  STATIC_ASSERT_EQ(do_append("hello", world), "hello world");
}

/// @test Invoking the `append` member function with a `string_view` increases the string length by the length of the
/// `string_view`, and copies the characters from the `string_view` onto the end of the string.
TEST(ConstexprInlineStringAppend, StringViewAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", string_view{" world"}), "hello world");
}

/// @test Invoking `append` with a `string_view` and a position appends the substring of that `string_view` starting at
/// the specified position on to the end of the string.
TEST(ConstexprInlineStringAppend, StringViewPosAtEndOfString) {
  constexpr string_view world{" world"};
  STATIC_ASSERT_EQ(do_append("hello", world, 1), "helloworld");
  STATIC_ASSERT_EQ(do_append("hello", world, world.size()), "hello");
}

/// @test Invoking `append` with a `string_view`, a position, and a count appends the substring of that `string_view` to
/// the specified length starting at the specified position on to the end of the string.
TEST(ConstexprInlineStringAppend, StringViewPosCountAtEndOfString) {
  constexpr string_view world{" world"};
  STATIC_ASSERT_EQ(do_append("hello", world, 1, 2), "hellowo");
  STATIC_ASSERT_EQ(do_append("hello", world, world.size(), world.size()), "hello");
}

/// @test Invoking `append` with a single character appends that character to the string
TEST(ConstexprInlineStringAppend, CharAtEndOfString) { STATIC_ASSERT_EQ(do_append("hello", 'x'), "hellox"); }

/// @test Invoking `append` with a count and a single character appends the specified number of copies of that character
/// to the string
TEST(ConstexprInlineStringAppend, CharRepeatingAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", 5, 'x'), "helloxxxxx");
}

/// @test Invoking `append` with an `inline_string` appends the contents of that string to the `inline_string` on which
/// `append` was invoked.
TEST(ConstexprInlineStringAppend, InlineStringThatDefinitelyFitsAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", (inline_string<default_size>{" world"})), "hello world");
}

/// @test Invoking `append` with an `inline_string` appends the contents of that string to the `inline_string` on which
/// `append` was invoked, even if the capacity of the appended string is large.
TEST(ConstexprInlineStringAppend, InlineStringMaybeFitsAtEndOfStringIfSizeFitsInRemainingCapacity) {
  STATIC_ASSERT_EQ(do_append("hello", (inline_string<default_size + 10>{" world"})), "hello world");
}

/// @test Invoking `append` with an initializer list of characters appends the characters from that initializer list to
/// the `inline_string` on which `append` was invoked.
TEST(ConstexprInlineStringAppend, InitializerListAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", std::initializer_list<char>{' ', 'w', 'o', 'r', 'l', 'd'}), "hello world");
}

/// Constexpr wrapper around inline_string<N>::+=()
/// @returns A copy of the input string that has had `+= rhs` called on it.
template <typename Rhs>
constexpr auto do_plus_assign(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs += rhs;
  return lhs;
}

/// @test Using `+=` to append a string literal to an `inline_string` adds the characters to the `inline_string`
TEST(ConstexprInlineStringPlusAssign, StringLiteralAtEndOfString) {
  STATIC_ASSERT_EQ(do_plus_assign("hello", " world"), "hello world");
}

/// @test Using `+=` to append a pointer to a NUL-terminated string to an `inline_string` adds the characters from the
/// NUL-terminated string to the `inline_string`
TEST(ConstexprInlineStringPlusAssign, CStringAtEndOfString) {
  constexpr auto* world = " world";
  STATIC_ASSERT_EQ(do_plus_assign("hello", world), "hello world");
}

/// @test Using `+=` to append a `string_view` to an `inline_string` adds the characters from the `string_view` to the
/// `inline_string`
TEST(ConstexprInlineStringPlusAssign, StringViewAtEndOfString) {
  STATIC_ASSERT_EQ(do_plus_assign("hello", string_view{" world"}), "hello world");
}

/// @test Using `+=` to append a character to an `inline_string` adds that character to the end of the string
TEST(ConstexprInlineStringPlusAssign, CharAtEndOfString) { STATIC_ASSERT_EQ(do_plus_assign("hello", 'x'), "hellox"); }

/// @test Using `+=` to append an `inline_string` to an `inline_string` adds the characters from the `inline_string`
/// that is the RHS operand to the  `inline_string` that is the LHS operand
TEST(ConstexprInlineStringPlusAssign, InlineStringThatDefinitelyFitsAtEndOfString) {
  STATIC_ASSERT_EQ(do_plus_assign("hello", (inline_string<default_size>{" world"})), "hello world");
}

/// @test Using `+=` to append an `inline_string` with a larger capacity to an `inline_string` adds the characters from
/// the `inline_string` that is the RHS operand to the  `inline_string` that is the LHS operand
TEST(ConstexprInlineStringPlusAssign, InlineStringMaybeFitsAtEndOfStringIfSizeFitsInRemainingCapacity) {
  STATIC_ASSERT_EQ(do_plus_assign("hello", (inline_string<default_size + 10>{" world"})), "hello world");
}

/// @test Using `+=` to append an initializer list of characters to an `inline_string` adds the characters from the
/// initializer list to the `inline_string`
TEST(ConstexprInlineStringPlusAssign, InitializerListAtEndOfString) {
  STATIC_ASSERT_EQ(do_plus_assign("hello", std::initializer_list<char>{' ', 'w', 'o', 'r', 'l', 'd'}), "hello world");
}

/// Constexpr wrapper for inline_string<default_size>.push_back()
/// @return The input string with push_back(arg) called on it.
constexpr auto do_push_back(inline_string<default_size> lhs, char a_char) -> inline_string<default_size> {
  lhs.push_back(a_char);
  return lhs;
}

/// @test Invoking `push_back` appends a character to an `inline_string`
TEST(ConstexprInlineStringPushBack, AppendsCharToEndOfString) {
  STATIC_ASSERT_EQ(do_push_back("hello", 'Z'), "helloZ");
}

/// constexpr wrapper for inline_string<default_size>::operator=
/// @return A copy of the input string with rhs assigned to it.
template <typename Rhs>
constexpr auto do_assignment(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs = rhs;
  return lhs;
}

/// @test Assigning to an `inline_string` from a single character replaces the string with a string of length one
/// holding that character
TEST(ConstexprInlineStringAssignment, CharReplacesStringWithRhs) { STATIC_ASSERT_EQ(do_assignment("hello", 'a'), "a"); }

/// @test Assigning to an `inline_string` from a string literal replaces the string with a string with the same length
/// and characters as the string literal
TEST(ConstexprInlineStringAssignment, StringLiteralReplacesStringWithRhs) {
  STATIC_ASSERT_EQ(do_assignment("hello", "goodbye"), "goodbye");
}

/// @test Assigning to an `inline_string` from a pointer to a NUL-terminated string replaces the string with a string
/// with the same length and characters as the NUL-terminated string
TEST(ConstexprInlineStringAssignment, CStrReplacesStringWithRhs) {
  constexpr auto* goodbye = "goodbye";
  STATIC_ASSERT_EQ(do_assignment("hello", goodbye), goodbye);
}

/// @test Assigning to an `inline_string` from a `string_view` replaces the string with a string with the same length
/// and characters as the `string_view`
TEST(ConstexprInlineStringAssignment, StringViewReplacesStringWithRhs) {
  constexpr string_view goodbye{"goodbye"};
  STATIC_ASSERT_EQ(do_assignment("hello", goodbye), goodbye);
}

/// @test Assigning to an `inline_string` from an initializer list of characters replaces the string with a string
/// with the same length and characters as the initializer list
TEST(ConstexprInlineStringAssignment, InitializerListReplacesStringWithRhs) {
  STATIC_ASSERT_EQ(do_assignment("hello", std::initializer_list<char>{'a', 'b', 'c'}), "abc");
}

/// @test Invoking `capacity` returns the capacity of the string
TEST(ConstexprInlineStringCapacity, IsMaxLength) {
  constexpr unsigned size = 123;
  constexpr inline_string<size> str;
  STATIC_ASSERT_EQ(str.capacity(), size);
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
TEST(ConstexprInlineStringResize, ToZeroIsEmptyString) {
  STATIC_ASSERT_TRUE(do_resize("hello", 0).empty());
  STATIC_ASSERT_TRUE(do_resize("hello world", 0).empty());
}

/// @test Invoking `resize` with a size less than or equal to the capacity (with or without a fill character) sets the
/// length of the string to the new size.
TEST(ConstexprInlineStringResize, ToNotGreaterThanMaxLengthResultsInSize) {
  STATIC_ASSERT_EQ(do_resize("hello", 1).size(), 1);
  STATIC_ASSERT_EQ(do_resize("hello", 1, 'a').size(), 1);

  STATIC_ASSERT_EQ(do_resize("hello", 4).size(), 4);
  STATIC_ASSERT_EQ(do_resize("hello", 4, 'a').size(), 4);

  STATIC_ASSERT_EQ(do_resize("hello", default_size).size(), default_size);
  STATIC_ASSERT_EQ(do_resize("hello", default_size, 'a').size(), default_size);
}

/// @test Invoking `resize` with a size less than the current size (with or without a fill character) truncates the
/// existing string to the specified length
TEST(ConstexprInlineStringResize, IfLessThanOrEqualCurrentSizeTruncatesToNewSize) {
  constexpr inline_string<default_size> original{"hello"};
  STATIC_ASSERT_EQ(do_resize(original, 1), original.substr(0, 1));
  STATIC_ASSERT_EQ(do_resize(original, 1, 'a'), original.substr(0, 1));
  STATIC_ASSERT_EQ(do_resize(original, original.size()), original.substr(0, original.size()));
  STATIC_ASSERT_EQ(do_resize(original, original.size(), 'a'), original.substr(0, original.size()));
}

/// @test Invoking `resize` with a size larger than the current size without a fill character extends the string to the
/// new size by padding with NUL characters
TEST(ConstexprInlineStringResize, IfGreaterThanCurrentSizeAndNoCharFillsRemainderWithNull) {
  constexpr inline_string<default_size> original{"hello"};
  STATIC_ASSERT_STREQ(do_resize(original, original.size() + 1).c_str(), original.c_str());
  STATIC_ASSERT_EQ(do_resize(original, original.size() + 1)[original.size()], '\0');
  STATIC_ASSERT_EQ(do_resize(original, original.size() + 10)[original.size() + 10], '\0');
}

/// @test Invoking `resize` with a size larger than the current size with a fill character extends the string to the
/// new size by padding with the specified fill character
TEST(ConstexprInlineStringResize, IfGreaterThanCurrentSizeFillsRemainderWithChar) {
  constexpr inline_string<default_size> original{"hello"};
  STATIC_ASSERT_EQ(do_resize(original, original.size() + 1, 'a'), original + string_view{"a"});
  STATIC_ASSERT_EQ(do_resize(original, original.size() + 5, 'a'), original + string_view{"aaaaa"});
}

/// @test Coverting an `inline_string` to a `string_view` sets the `data` and `size` of the `string_view` to the results
/// of `c_str` and `size` on the `inline_string`
TEST(ConstexprInlineStringConversion, StringViewConversion) {
  constexpr inline_string<default_size> str{"hello"};
  STATIC_ASSERT_EQ(static_cast<string_view>(str).data(), str.c_str());
  STATIC_ASSERT_EQ(static_cast<string_view>(str).size(), str.size());
}

/// Constexpr wrapper for inline_string<default_size>.clear()
/// @return The input string with .clear() called on it.
constexpr auto do_clear(inline_string<default_size> str) -> inline_string<default_size> {
  str.clear();
  return str;
}

/// @test Invoking `clear` changes the string to an empty string
TEST(ConstexprInlineStringClear, ResultsInEmptyString) { STATIC_ASSERT_TRUE(do_clear("hello").empty()); }

/// @test `length` and `size` return the same value
TEST(ConstexprInlineStringLength, IsSameAsSize) {
  constexpr inline_string<default_size> str{"hello"};
  STATIC_ASSERT_EQ(str.length(), str.size());
}

/// @test 'front' returns a reference to the first character in the string
TEST(ConstexprInlineStringFront, GetsFirstCharacter) {
  constexpr inline_string<default_size> str{"hello"};
  ::testing::StaticAssertTypeEq<decltype(str.front()), char const&>();
  STATIC_ASSERT_EQ(str.front(), str[0]);
  STATIC_ASSERT_EQ(&str.front(), str.data());
}

/// @test `back` returns a reference to the last character in the string
TEST(ConstexprInlineStringBack, GetsLastCharacter) {
  constexpr inline_string<default_size> str{"hello"};
  ::testing::StaticAssertTypeEq<decltype(str.back()), char const&>();
  STATIC_ASSERT_EQ(str.back(), str[str.size() - 1]);
  STATIC_ASSERT_EQ(&str.back(), &str[str.size() - 1]);
}

/// @brief Helper function to use @c assign in a @c constexpr context
/// @tparam MaxLength the max length of the string
/// @tparam Args the types of the arguments to assign
/// @param inline_str The source string
/// @param args The arguments to assign
/// @return The result of invoking @c assign
template <std::size_t MaxLength, typename... Args>
constexpr auto constexpr_assign(inline_string<MaxLength> inline_str, Args&&... args) -> inline_string<MaxLength> {
  return inline_str.assign(std::forward<Args>(args)...);
}

/// @test `assign` can be invoked in a `constexpr` context to set the target string to the specified source
TEST(ConstexprInlineString, ConstexprAssignment) {
  constexpr inline_string<default_size> inline_str("initial");
  constexpr string_view str_view("string_view");
  constexpr char const* char_ptr("const char pointer");
  constexpr char char_literal = 'X';
  STATIC_ASSERT_EQ(do_assignment(inline_str, str_view), str_view);
  STATIC_ASSERT_EQ(do_assignment(inline_str, char_ptr), char_ptr);
  STATIC_ASSERT_EQ(do_assignment(inline_str, char_literal), "X");
  STATIC_ASSERT_EQ(constexpr_assign(inline_str, static_cast<std::size_t>(7), char_literal), "XXXXXXX");
  STATIC_ASSERT_EQ(constexpr_assign(inline_str, str_view, static_cast<std::size_t>(3)), str_view.substr(3));
  STATIC_ASSERT_EQ(
      constexpr_assign(inline_str, str_view, static_cast<std::size_t>(4), static_cast<std::size_t>(2)),
      str_view.substr(4, 2)
  );
  STATIC_ASSERT_EQ(constexpr_assign(inline_str, char_ptr, static_cast<std::size_t>(4)), (string_view{char_ptr, 4}));
  STATIC_ASSERT_EQ(do_assignment(inline_str, std::initializer_list<char>{'X', 'Y', 'Z', '!'}), "XYZ!");
}

/// constexpr wrapper around str.insert
/// @return str.insert(args...);
template <typename... Args>
constexpr auto do_insert(inline_string<default_size> str, Args&&... args) -> inline_string<default_size> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  return str.insert(std::forward<Args>(args)...);
}

/// @test `insert` can be invoked in a `constexpr` context to perform the desired insertions
TEST(ConstexprInlineStringInsert, WorksInConstexpr) {
  constexpr inline_string<default_size> inline_str{"abcdef"};
  STATIC_ASSERT_EQ(do_insert(inline_str, static_cast<std::size_t>(2), "XXX"), "abXXXcdef");
  STATIC_ASSERT_EQ(
      do_insert(inline_str, static_cast<std::size_t>(2), "XXXYYY", static_cast<std::size_t>(3)),
      "abXXXcdef"
  );
  STATIC_ASSERT_EQ(do_insert(inline_str, static_cast<std::size_t>(2), string_view{"XXX"}), "abXXXcdef");
  STATIC_ASSERT_EQ(
      do_insert(inline_str, static_cast<std::size_t>(2), string_view{"XXXYYYZZZ"}, static_cast<std::size_t>(3)),
      "abYYYZZZcdef"
  );
  STATIC_ASSERT_EQ(
      do_insert(
          inline_str,
          static_cast<std::size_t>(2),
          string_view{"XXXYYYZZZ"},
          static_cast<std::size_t>(3),
          static_cast<std::size_t>(4)
      ),
      "abYYYZcdef"
  );
  STATIC_ASSERT_EQ(do_insert(inline_str, static_cast<std::size_t>(2), static_cast<std::size_t>(4), 'Z'), "abZZZZcdef");
}

/// @test Invoking `insert` with an iterator and an iterator range works in a `constexpr` context to perform the
/// expected insertions
CONSTEXPR_TEST(ConstexprInlineStringInsert, IteratorInsertionInConstexpr) {
  constexpr inline_string<default_size> original_str{"abcdef"};

  inline_string<default_size> one_x{original_str};
  one_x.insert(one_x.begin() + static_cast<std::ptrdiff_t>(2), 'X');
  CONSTEXPR_ASSERT_EQ(one_x, "abXcdef");

  inline_string<default_size> four_zs{original_str};
  four_zs.insert(four_zs.begin() + static_cast<std::ptrdiff_t>(2), static_cast<std::size_t>(4), 'Z');
  CONSTEXPR_ASSERT_EQ(four_zs, "abZZZZcdef");

  constexpr array<char, 3> arr{'X', 'Y', 'Z'};
  inline_string<default_size> from_arr{original_str};
  from_arr.insert(from_arr.begin() + static_cast<std::ptrdiff_t>(2), arr.begin(), arr.end());
  CONSTEXPR_ASSERT_EQ(from_arr, "abXYZcdef");

  inline_string<default_size> from_ilist{original_str};
  from_ilist.insert(from_ilist.begin() + static_cast<std::ptrdiff_t>(2), std::initializer_list<char>{'A', 'B', 'C'});
  CONSTEXPR_ASSERT_EQ(from_ilist, "abABCcdef");
}

/// @brief Helper function to use @c erase in a @c constexpr context
/// @tparam MaxLength the max length of the string
/// @tparam Args the types of the arguments to erase
/// @param inline_str The source string
/// @param args The arguments to erase
/// @return The result of invoking @c erase
template <std::size_t MaxLength, typename... Args>
constexpr auto constexpr_erase(inline_string<MaxLength> str, Args&&... args) -> inline_string<MaxLength> {
  return str.erase(std::forward<Args>(args)...);
}

/// @test Invoking `erase` in a `constexpr` context works as expected
TEST(ConstexprInlineStringErase, EraseInConstexprContext) {
  constexpr inline_string<default_size> inline_str{"abcdefg"};
  // NOLINTNEXTLINE(readability-container-size-empty)
  STATIC_ASSERT_EQ(constexpr_erase(inline_str), "");
  STATIC_ASSERT_EQ(constexpr_erase(inline_str, static_cast<std::size_t>(3)), "abc");
  STATIC_ASSERT_EQ(constexpr_erase(inline_str, static_cast<std::size_t>(4), static_cast<std::size_t>(2)), "abcdg");
}

/// @test Invoking `max_size` returns the capacity of the string
TEST(ConstexprInlineString, MaxSizeEqualsCapacity) {
  constexpr auto size1 = 42;
  STATIC_ASSERT_EQ(inline_string<size1>().max_size(), inline_string<size1>().capacity());
  STATIC_ASSERT_EQ(inline_string<size1>::max_size(), inline_string<size1>::capacity());
}

/// @brief Helper function to use @c erase in a @c constexpr context
/// @tparam MaxLength the max length of the string
/// @tparam Args the types of the arguments to erase
/// @param inline_str The source string
/// @param args The arguments to erase
/// @return The result of invoking @c erase
template <std::size_t MaxLength, typename... Args>
constexpr auto constexpr_iterator_erase(inline_string<MaxLength> str, Args&&... args) -> inline_string<MaxLength> {
  str.erase((str.begin() + std::forward<Args>(args))...);
  return str;
}

/// @test Invoking `erase` in a `constexpr` context with an iterator range works as expected
TEST(ConstexprInlineStringErase, EraseInConstexpr) {
  constexpr inline_string<default_size> inline_str{"abcdefg"};
  STATIC_ASSERT_EQ(constexpr_iterator_erase(inline_str, 2), "abdefg");
  STATIC_ASSERT_EQ(constexpr_iterator_erase(inline_str, 3, 5), "abcfg");
  STATIC_ASSERT_EQ(constexpr_iterator_erase(inline_str, 7), "abcdefg");
}

/// @brief Helper function to use @c pop_back in a @c constexpr context
/// @param inline_str The source string
/// @return The resulting string after invoking @c pop_back
template <std::size_t MaxLength>
constexpr auto constexpr_pop_back(inline_string<MaxLength> str) -> inline_string<MaxLength> {
  str.pop_back();
  return str;
}

/// @test Invoking `pop_back` in a `constexpr` context works as expected
TEST(ConstexprInlineStringPopBack, PopBackInConstexpr) {
  constexpr inline_string<default_size> inline_str{"hello"};
  STATIC_ASSERT_EQ(constexpr_pop_back(inline_str), "hell");
}

/// @test `npos` is equal to the max value representable by `std::size_t`
TEST(ConstexprInlineString, Npos) {
  ::testing::StaticAssertTypeEq<decltype(inline_string<default_size>::npos), std::size_t const>();
  STATIC_ASSERT_EQ(inline_string<default_size>::npos, std::numeric_limits<std::size_t>::max());
}

/// constexpr wrapper for inline_string.replace
/// @return str.replace(args...)
template <typename... Args>
constexpr auto do_replace(inline_string<default_size> str, Args&&... args) -> inline_string<default_size> {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  return str.replace(std::forward<Args>(args)...);
}

/// @test Invoking `replace` in a `constexpr` context has the expected behaviour
TEST(ConstexprInlineStringReplace, ReplaceInConstexpr) {
  constexpr inline_string<default_size> inline_str{"abcdefg"};
  STATIC_ASSERT_EQ(do_replace(inline_str, 3U, 2U, "XYZ"), "abcXYZfg");
  STATIC_ASSERT_EQ(do_replace(inline_str, 3U, 2U, "XXXYYY", 3U), "abcXXXfg");
  STATIC_ASSERT_EQ(do_replace(inline_str, 3U, 2U, string_view{"XYZ"}), "abcXYZfg");
  STATIC_ASSERT_EQ(do_replace(inline_str, 3U, 2U, string_view{"XXXYYYZZZ"}, 3U), "abcYYYZZZfg");
  STATIC_ASSERT_EQ(do_replace(inline_str, 3U, 2U, string_view{"XXXYYYZZZ"}, 3U, 4U), "abcYYYZfg");
  STATIC_ASSERT_EQ(do_replace(inline_str, 3U, 2U, 5U, 'Z'), "abcZZZZZfg");
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

/// @test Invoking `replace` with an iterator range in a `constexpr` context works as expected
TEST(ConstexprInlineStringReplace, ReplaceWithIteratorInConstexpr) {
  constexpr inline_string<default_size> inline_str{"abcdefg"};
  constexpr inline_string<default_size> inline_str_2{"XY"};

  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, "XYZ"), "abcdXYZg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, string_view{"XYZ"}), "abcdXYZg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, "XXXYYY", 3U), "abcdXXXg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, string_view{"XXXYYYZZZ"}, 3U), "abcdYYYZZZg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, string_view{"XXXYYYZZZ"}, 3U, 4U), "abcdYYYZg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, 2U, 'Z'), "abcdZZg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, std::initializer_list<char>{'A', 'B', 'C', 'D'}), "abcdABCDg");
  STATIC_ASSERT_EQ(do_iterator_replace(inline_str, 4, 6, inline_str_2.begin(), inline_str_2.end()), "abcdXYg");
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
TEST(ConstexprInlineStringCopy, IfNLessThanOrEqualSizeCopiesNCharactersToDest) {
  STATIC_ASSERT_EQ(do_copy("hello", "world", 0U).first, "world");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 0U).second, 0);
  STATIC_ASSERT_EQ(do_copy("hello", "world", 3U).first, "helld");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 3U).second, 3);
  STATIC_ASSERT_EQ(do_copy("hello", "world", 5U).first, "hello");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 5U).second, 5);
}

/// @test Invoking `copy` with a count greater than the length of the string copies the whole string into the target
/// buffer
TEST(ConstexprInlineStringCopy, IfNGreaterSizeCopiesSizeCharactersToDest) {
  STATIC_ASSERT_EQ(do_copy("hello", "world", 6U).first, "hello");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 6U).second, 5);
  STATIC_ASSERT_EQ(do_copy("hello", "world", 100U).first, "hello");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 100U).second, 5);
}

/// @test Invoking `copy` with a position and count copies the specified number of characters from the specified
/// position into the target buffer
TEST(ConstexprInlineStringCopy, IfPosLessThanSizeCopiesUpToNCharactersFromPosToDest) {
  STATIC_ASSERT_EQ(do_copy("hello", "world", 0U, 0U).first, "world");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 0U, 0U).second, 0);
  STATIC_ASSERT_EQ(do_copy("hello", "world", 3U, 0U).first, "helld");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 3U, 0U).second, 3);
  STATIC_ASSERT_EQ(do_copy("hello", "world", 3U, 3U).first, "lorld");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 3U, 3U).second, 5 - 3);
}

/// @test Invoking `copy` with a position and count where the position is greater than the size of the string does
/// nothing
TEST(ConstexprInlineStringCopy, IfPosGreaterSizeCopies0CharactersToDest) {
  STATIC_ASSERT_EQ(do_copy("hello", "world", 6U, 6U).first, "world");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 6U, 6U).second, 0);
  STATIC_ASSERT_EQ(do_copy("hello", "world", 6U, 100U).first, "world");
  STATIC_ASSERT_EQ(do_copy("hello", "world", 6U, 100U).second, 0);
}

/// @test Invoking `substr` without any arguments returns a copy of the whole string
TEST(ConstexprInlineStringSubstr, WithoutPosStartsSubstrAtBegin) {
  constexpr inline_string<default_size> inline_str{"abcdefgh"};
  ::testing::StaticAssertTypeEq<decltype(inline_str.substr()), arene::base::remove_cvref_t<decltype(inline_str)>>();
  STATIC_ASSERT_EQ(inline_str.substr(), "abcdefgh");
}

/// @test Invoking `substr` with a position returns a new string holding the characters of the source string starting at
/// the specified position
TEST(ConstexprInlineStringSubstr, WithPosStartsSubstrAtPos) {
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<inline_string<default_size>>().substr(5)), inline_string<default_size>>(
      );
  STATIC_ASSERT_EQ(inline_string<default_size>{"abcdefgh"}.substr(3), "defgh");
}

/// @test Invoking `substr` with a position and a count returns a new string holding the characters of the source string
/// starting at the specified position, up to the specified number of characters
TEST(ConstexprInlineStringSubstr, WithPosAndCountStartsSubstrAtPosExtendsCountClampedToSize) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>>().substr(5, 2)),
      inline_string<default_size>>();
  STATIC_ASSERT_TRUE(inline_string<default_size>{"abcdefgh"}.substr(0, 0).empty());
  STATIC_ASSERT_TRUE(inline_string<default_size>{"abcdefgh"}.substr(3, 0).empty());
  STATIC_ASSERT_EQ(inline_string<default_size>{"abcdefgh"}.substr(3, 4), "defg");
  STATIC_ASSERT_EQ(inline_string<default_size>{"abcdefgh"}.substr(3, 1000), "defgh");
}

/// @test Invoking `substr` with a position greater than the length of the string returns an empty string
TEST(ConstexprInlineStringSubstr, IfPosGESizeIsEmptyString) {
  constexpr inline_string<default_size> inline_str{"abcdefgh"};
  ::testing::StaticAssertTypeEq<decltype(inline_str.substr(5)), arene::base::remove_cvref_t<decltype(inline_str)>>();
  STATIC_ASSERT_TRUE(inline_str.substr(inline_str.size()).empty());
  STATIC_ASSERT_TRUE(inline_str.substr(inline_str.size() + 100).empty());
  STATIC_ASSERT_TRUE(inline_str.substr(inline_str.size(), 0).empty());
  STATIC_ASSERT_TRUE(inline_str.substr(inline_str.size() + 100, 0).empty());
  STATIC_ASSERT_TRUE(inline_str.substr(inline_str.size(), 10).empty());
  STATIC_ASSERT_TRUE(inline_str.substr(inline_str.size() + 100, 10).empty());
}

/// @test Invoking `find` with the whole string returns zero
TEST(ConstexprInlineStringFind, FindSelf) {
  constexpr inline_string<default_size> inline_str{"hello world"};
  STATIC_ASSERT_EQ(inline_str.find(inline_str), 0);
}

/// @test Invoking `find` with a string not contained returns `npos`
TEST(ConstexprInlineStringFind, UnmatchingStringNotFound) {
  constexpr inline_string<default_size> inline_str{"hello world"};
  constexpr inline_string<default_size> inline_str_2{"goodbye"};
  STATIC_ASSERT_EQ(inline_str.find(inline_str_2), inline_string<default_size>::npos);
}

/// @test Invoking `find` with a string that is a substring returns the first index of that substring
TEST(ConstexprInlineStringFind, SubstringFound) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find(inline_string<default_size>{"wor"}), 6);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find(string_view{}), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{}.find(string_view{}), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find("o"_asv), 4);
}

/// @test Invoking `find` with a string that is a substring and a position returns the first index of that substring
/// after the specified position
TEST(ConstexprInlineStringFind, FindWithPosition) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find("o"_asv, 5), 7);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find("hello"_asv, 1), inline_string<default_size>::npos);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.find("hello"_asv, inline_string<default_size>::npos),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find` with a single character returns the first index of the character in the string
TEST(ConstexprInlineStringFind, FindSingleChar) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.find('t'), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.find('s'), 2);
}

/// @test Invoking `find` with a single character and a position returns the first index of the character in the string
/// found at or after the specified position
TEST(ConstexprInlineStringFind, FindSingleCharWithStartPosition) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.find('t', 1), 3);
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.find('e', 2), inline_string<default_size>::npos);
}

/// @test Invoking `find` with a pointer to a NUL-terminated string returns the first index of the characters in the
/// NUL-terminated string in the string
TEST(ConstexprInlineStringFind, SubstringFoundWithCharPointer) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find(inline_string<default_size>{"wor"}.c_str()), 6);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find(static_cast<char const*>("")), 0);
  STATIC_ASSERT_EQ(string_view{}.find(static_cast<char const*>("")), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find(static_cast<char const*>("o")), 4);
}

/// @test Invoking `find` with a character pointer and a length returns the first index of the string formed from the
/// specified number of characters at the specified pointer
TEST(ConstexprInlineStringFind, FindWithPositionWithCharPointer) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.find(static_cast<char const*>("o"), 5), 7);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.find(static_cast<char const*>("hello"), 1),
      inline_string<default_size>::npos
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .find(static_cast<char const*>("hello"), inline_string<default_size>::npos),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find` with a character and a position and count returns the first index from the specified position
/// of the string of the specified length from the character pointer
TEST(ConstexprInlineStringFind, FindWithPositionAndCount) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .find(inline_string<default_size>{"hello world"}.substr(4).c_str(), 5, 1),
      7
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello hell"}.find(inline_string<default_size>{"hello hell"}.c_str(), 1, 4),
      6
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.find("hello", inline_string<default_size>::npos, 3),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `rfind` with the whole string returns zero
TEST(ConstexprInlineStringRFind, ReverseFindSelf) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{"hello world"}), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{}.rfind(inline_string<default_size>{}), 0);
}

/// @test Invoking `rfind` with a string that is not present returns `npos`
TEST(ConstexprInlineStringRFind, UnmatchingStringNotFoundInReverse) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello"}.rfind(inline_string<default_size>{"goodbye"}),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `rfind` with a substring returns the index of that substring
TEST(ConstexprInlineStringRFind, SubstringFoundInReverse) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{"wor"}), 6);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{}),
      inline_string<default_size>{"hello world"}.length()
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{}.rfind(string_view{}), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("o"_asv), 7);
}

/// @test Invoking `rfind` with a substring and a position returns the index of that substring that is less than or
/// equal to the specified position
TEST(ConstexprInlineStringRFind, ReverseFindWithPosition) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("o"_asv, 5), 4);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("world"_asv, 5), inline_string<default_size>::npos);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind("hello"_asv, inline_string<default_size>::npos), 0);
}

/// @test Invoking `rfind` with a single character returns the index of the last instance of that character
TEST(ConstexprInlineStringRFind, ReverseFindSingleChar) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.rfind('t'), 3);
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.rfind('s'), 2);
}

/// @test Invoking `rfind` with a single character and a position returns the index of the last instance of that
/// character less than or equal to the specified position
TEST(ConstexprInlineStringRFind, ReverseFindSingleCharWithStartPosition) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.rfind('t', 1), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"test"}.rfind('s', 1), inline_string<default_size>::npos);
}

/// @test Invoking `rfind` with a pointer to a NUL-terminated string returns the index of the last instance of the
/// pointed-to string
TEST(ConstexprInlineStringRFind, SubstringFoundWithCharPointerInReverse) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(inline_string<default_size>{"wor"}.c_str()), 6);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("")),
      inline_string<default_size>{"hello world"}.length()
  );
  STATIC_ASSERT_EQ(string_view{}.rfind(static_cast<char const*>("")), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("o")), 7);
}

/// @test Invoking `rfind` with a pointer to a NUL-terminated string and a position returns the index of the last
/// instance of the pointed-to string less than the specified position
TEST(ConstexprInlineStringRFind, ReverseFindWithPositionWithCharPointer) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("o"), 5), 4);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind(static_cast<char const*>("world"), 1),
      inline_string<default_size>::npos
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .rfind(static_cast<char const*>("hello"), inline_string<default_size>::npos),
      0
  );
}

/// @test Invoking `rfind` with a character pointer and a position and length returns the index of the last
/// instance of the string consisting of the specified number of characters from the pointer that is less than or equal
/// to the position
TEST(ConstexprInlineStringRFind, ReverseFindWithPositionAndCount) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}
          .rfind(inline_string<default_size>{"hello world"}.substr(4).c_str(), 5, 1),
      4
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hell hello"}.rfind(inline_string<default_size>{"hell hello"}.c_str(), 7, 4),
      5
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world"}.rfind("good", inline_string<default_size>::npos, 3),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a list of characters that aren't present returns `npos`
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfNonMatchingList) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abc"}),
      inline_string<default_size>::npos
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"banana"}.find_first_of("fruit"), inline_string<default_size>::npos);
  STATIC_ASSERT_EQ(inline_string<default_size>{"banana"}.find_first_of(""), inline_string<default_size>::npos);
}

/// @test Invoking `find_first_of` with a list of characters where at least one is present returns the index of the
/// first character in the string that is in the list
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfMatchingList) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abcdef"}), 1);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"hw"}), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"woven planet"}.find_first_of(inline_string<default_size>{"abcdef"}), 3);
}

/// @test Invoking `find_first_of` with a list of characters and an offset where at least one is present returns the
/// index of the first character in the string that is in the list at or after the specified offset
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("abcde"_asv, 2), 10);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("hw"_asv, 1), 6);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of("fg%"_asv, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a list of characters specified by a NUL-terminated string where at least one is
/// present returns the index of the first character in the string that is in the list
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfMatchingListWithCharPointer) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abcdef"}.c_str()),
      1
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"hw"}.c_str()),
      0
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("lw"), 2);
}

/// @test Invoking `find_first_of` with a list of characters specified by a NUL-terminated string and an offset where at
/// least one is present returns the index of the first character in the string that is in the list at or after the
/// specified offset
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfWithOffsetWithCharPointer) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(static_cast<char const*>("abcde"), 2), 10);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of(static_cast<char const*>("hw"), 1), 6);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(static_cast<char const*>("fg%"), 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a single character returns the index of that character, or `npos` if it is not
/// found
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfWithCharacter) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('e'), 1);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('h'), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('a'), inline_string<default_size>::npos);
}

/// @test Invoking `find_first_of` with a single character and an offset returns the index of that character at or after
/// the offset, or `npos` if it is not found
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfCharacterWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of('l', 5), 9);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of('h', 1),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a list of characters specified by a string literal and an offset and length
/// where at least one is present returns the index of the first character in the string that is in the list specified
/// as the string of the specified length from the character pointer, at or after the specified offset in the searched
/// string
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfMatchingListWithOffsetAndLength) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_of("abcdef", 0, 4), 10);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of("helow", 2, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_of` with a list of characters specified by a character pointer and an offset and length
/// where at least one is present returns the index of the first character in the string that is in the list specified
/// as the string of the specified length from the character pointer, at or after the specified offset in the searched
/// string
TEST(ConstexprInlineStringFindFirstOf, FindFirstOfMatchingListWithCharPointerAndLength) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"abcdef"}.c_str(), 0, 4),
      10
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_of(inline_string<default_size>{"hw"}.c_str(), 2, 1),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_not_of` with a list of characters returns the index of the first character not in the
/// list, or `npos` if all the characters in the string are in the list.
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOf) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of("abc"_asv), 0);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(inline_string<default_size>{"hello world!"}),
      inline_string<default_size>::npos
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"woven planet"}.find_first_not_of(string_view{}), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"woven planet"}.find_first_not_of("avowed"), 4);
}

/// @test Invoking `find_first_not_of` with a list of characters and an offset returns the index of the first character
/// not in the list after the offset, or `npos` if all the characters in the string after the offset are in the list.
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOfWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of("abcde"_asv, 2), 2);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of(" elo"_asv, 1), 6);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of("world! "_asv, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a NUL-terminated string returns the index
/// of the first character not in the list, or `npos` if all the characters in the string are in the list.
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOfMatchingListWithCharPointer) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(inline_string<default_size>{"abcdef"}.c_str()),
      0
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(inline_string<default_size>{"hello world!"}.c_str()
      ),
      inline_string<default_size>::npos
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of("helo"), 5);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a NUL-terminated string and an offset
/// returns the index of the first character not in the list at or after the offset, or `npos` if all the characters in
/// the string after the offset are in the list.
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOfWithOffsetWithCharPointer) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(static_cast<char const*>("abcde"), 2),
      2
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(static_cast<char const*>("elo "), 1),
      6
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_first_not_of(static_cast<char const*>("world! %^zz"), 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_first_not_of` with a single character returns the index of the first character
/// not equal to that character, or `npos` if all the characters in the string are the same as the character
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOfWithCharacter) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('e'), 0);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('h'), 1);
  STATIC_ASSERT_EQ(inline_string<default_size>("a").find_first_not_of('a'), inline_string<default_size>::npos);
  STATIC_ASSERT_EQ(inline_string<default_size>{"aaaaab"}.find_first_not_of('a'), 5);
}

/// @test Invoking `find_first_not_of` with a single character and an offset returns the index of the first character
/// not equal to the character at or after the offset
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOfCharacterWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('h', 5), 5);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_first_not_of('l', 2), 4);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a character pointer and an offset and
/// length returns the index of the first character not in the string of characters of the specified length from the
/// pointer after the offset, or `npos` if all the characters in the string at or after the offset are in the list.
TEST(ConstexprInlineStringFindFirstNotOf, FindFirstNotOfMatchingListWithCharPointerAndLength) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}
          .find_first_not_of(inline_string<default_size>{"abcdef"}.c_str(), 1, 4),
      1
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}
          .find_first_not_of(inline_string<default_size>{"eorld! w"}.c_str(), 1, 1),
      2
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"xxcccdddyyy"}.find_first_not_of("abcdef", 2, 3), 5);
}

/// @test Invoking `find_last_of` with a list of characters returns the index of the last character in the string that
/// is in the list, or `npos` if none of the characters are in the list
TEST(ConstexprInlineStringFindLastOf, FindLastOf) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv), 15);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(inline_string<default_size>{"goodbye everybody!"}),
      inline_string<default_size>{"goodbye everybody!"}.length() - 1
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"xxAxxxA"}.find_last_of("yzx"_asv), 5);
  STATIC_ASSERT_EQ(inline_string<default_size>{"xxAxxxA"}.find_last_of("abxd"), 5);
  STATIC_ASSERT_EQ(inline_string<default_size>{"xxAxxxA"}.find_last_of(""_asv), inline_string<default_size>::npos);
}

/// @test Invoking `find_last_of` with a list of characters and an offset returns the index of the last character in the
/// string at or before the offset that is in the list, or `npos` if none of the characters at or before the offset are
/// in the list
TEST(ConstexprInlineStringFindLastOf, FindLastOfWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv, 8), 4);
  STATIC_ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv, 3), 3);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of("abcd"_asv, 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a list of characters specified by a NUL-terminated string returns the index of
/// the last character in the string that is in the list, or `npos` if none of the characters are in the list
TEST(ConstexprInlineStringFindLastOf, FindLastOfWithCharPtrConversion) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd")),
      15
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(inline_string<default_size>{"goodbye everybody!"}),
      inline_string<default_size>{"goodbye everybody!"}.length() - 1
  );
}

/// @test Invoking `find_last_of` with a list of characters specified by a NUL-terminated string and an offset returns
/// the index of the last character in the string at or before the offset that is in the list, or `npos` if none of the
/// characters before the offset are in the list
TEST(ConstexprInlineStringFindLastOf, FindLastOfWithOffsetWithCharPtrConversion) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd"), 8),
      4
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd"), 3),
      3
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcd"), 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a list of characters specified by a character pointer and an offset and a count
/// returns the index of the last character in the string at or before the offset that is in a list made of the first N
/// characters from the character pointer, or `npos` if none of the characters before the offset are in the list
TEST(ConstexprInlineStringFindLastOf, FindLastOfWithOffsetAndCountWithCharPtrConversion) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcde"), 8, 4),
      4
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of("gxye", 8, 2), 0);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of(static_cast<char const*>("abcdefg"), 2, 4),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a single character returns the last index of that character, or `npos` if it is
/// not found
TEST(ConstexprInlineStringFindLastOf, FindLastOfChar) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of('d'), 15);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of('x'),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_of` with a single character and an offset returns the index of that character at or before
/// the offset, or `npos` if it is not found
TEST(ConstexprInlineStringFindLastOf, FindLastOfCharWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"goodbye everybody!"}.find_last_of('d', 8), 3);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"goodbye everybody!"}.find_last_of('y', 2),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters returns the index of the last character not in the
/// list, or `npos` if all the characters in the string are in the list.
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOf) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of("abc"_asv),
      inline_string<default_size>{"hello world!"}.length() - 1
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"hello world!"}),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters and an offset returns the index of the last character
/// not in the list at or before the offset, or `npos` if all the characters in the string before the offset are in the
/// list.
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOfWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of("abcde"_asv, 10), 9);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of("elowr"_asv, 9), 5);
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of("whelo "_asv, 7),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a NUL-terminated string returns the index
/// of the last character not in the list, or `npos` if all the characters in the string are in the list.
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOfMatchingListWithCharPointer) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"abcdef"}.c_str()),
      inline_string<default_size>{"hello world!"}.length() - 1
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"hello world!"}.c_str()),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a NUL-terminated string and an offset
/// returns the index of the last character not in the list at or before the offset, or `npos` if all the characters in
/// the string before the offset are in the list.
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOfWithOffsetWithCharPointer) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(static_cast<char const*>("abcde"), 10),
      9
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(static_cast<char const*>("elowr"), 9),
      5
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(static_cast<char const*>("whelo "), 7),
      inline_string<default_size>::npos
  );
}

/// @test Invoking `find_last_not_of` with a single character returns the index of the last character
/// not equal to that character, or `npos` if all the characters in the string are the same as the character
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOfWithCharacter) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of('e'),
      inline_string<default_size>{"hello world!"}.length() - 1
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of('!'),
      inline_string<default_size>{"hello world!"}.length() - 2
  );
  STATIC_ASSERT_EQ(inline_string<default_size>("a").find_last_not_of('a'), inline_string<default_size>::npos);
}

/// @test Invoking `find_last_not_of` with a single character and an offset returns the index of the last character
/// not equal to the character at or before the offset
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOfCharacterWithOffset) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of('h', 5), 5);
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello world!"}.find_last_not_of('l', 3), 1);
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a character pointer and an offset and
/// length returns the index of the last character not in the string of characters of the specified length from the
/// pointer at or before the offset, or `npos` if all the characters in the string at or before the offset are in the
/// list.
TEST(ConstexprInlineStringFindLastNotOf, FindLastNotOfMatchingListWithCharPointerAndLength) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}.find_last_not_of(inline_string<default_size>{"abcdef"}.c_str(), 1, 4),
      1
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"hello world!"}
          .find_last_not_of(inline_string<default_size>{"eorld! w"}.c_str(), 1, 1),
      0
  );
  STATIC_ASSERT_EQ(inline_string<default_size>{"aaaaayyyxxaaa"}.find_last_not_of("abcx", 11, 3), 9);
}

/// @test `starts_with` returns `true` if and only if the string starts with the specified characters
TEST(ConstexprInlineStringStartsWith, StartsWith) {
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"hello"}));
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"hell"}));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"goodbye"}));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with(string_view{"goodbye"}));
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with(inline_string<default_size>{"hell"}.c_str()));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with("hello world"));

  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.starts_with('h'));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.starts_with('x'));
  STATIC_ASSERT_FALSE(inline_string<default_size>{}.starts_with('x'));

  STATIC_ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().starts_with('h')));
  STATIC_ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().starts_with("h")));
  STATIC_ASSERT_TRUE(
      noexcept(std::declval<inline_string<default_size>>().starts_with(std::declval<inline_string<default_size>>()))
  );
}

/// @test `ends_with` returns `true` if and only if the string ends with the specified characters
TEST(ConstexprInlineStringEndsWith, EndsWith) {
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"hello"}));
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"ello"}));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"goodbye"}));
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(string_view{"ello"}));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.ends_with(string_view{"goodbye"}));
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with(inline_string<default_size>{"ello"}.c_str()));
  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with("ello"));
  STATIC_ASSERT_FALSE(inline_string<default_size>{}.ends_with("goodbye"));

  STATIC_ASSERT_TRUE(inline_string<default_size>{"hello"}.ends_with('o'));
  STATIC_ASSERT_FALSE(inline_string<default_size>{"hello"}.ends_with('x'));
  STATIC_ASSERT_FALSE(inline_string<default_size>{}.ends_with('x'));

  STATIC_ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().ends_with('h')));
  STATIC_ASSERT_TRUE(noexcept(std::declval<inline_string<default_size>>().ends_with("h")));
  STATIC_ASSERT_TRUE(
      noexcept(std::declval<inline_string<default_size>>().ends_with(std::declval<inline_string<default_size>>()))
  );
}

/// @test Concatenating strings with `+` yields a string with the appropriate contents
TEST(ConstexprInlineStringOperatorPlus, WithInlineStringsCreatesNewStringThatIsConcatinationOfInputStrings) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello"} + inline_string<default_size>{" world"}, "hello world");
}

/// @test Concatenating strings with `+` yields a string with the sum of the capacities
TEST(ConstexprInlineStringOperatorPlus, WithInlineStringsReturnedStringCapacityIsSumOfCapacities) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>>() + std::declval<inline_string<default_size>>()),
      inline_string<default_size + default_size>>();
}

/// @test Concatenating an `inline_string` and a `string_view` with `+` yields a string with the appropriate contents
TEST(ConstexprInlineStringOperatorPlus, WithStringViewCreatesNewStringThatIsConcatinationOfInputStrings) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello"} + string_view{" world"}, "hello world");
  STATIC_ASSERT_EQ(string_view{"hello"} + inline_string<default_size>{" world"}, "hello world");
}

/// @test Concatenating an `inline_string` and a `string_view` with `+` yields a string with the capacity of the
/// supplied `inline_string`
TEST(ConstexprInlineStringOperatorPlus, WithStringViewReturnedStringCapacityIsUnchanged) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<inline_string<default_size>>() + std::declval<string_view>()),
      inline_string<default_size>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<string_view>() + std::declval<inline_string<default_size>>()),
      inline_string<default_size>>();
}

/// @test `compare` returns greater than zero when comparing a non-empty string against an empty string
TEST(ConstexprInlineStringCompare, AgainstEmptyIsGreater) {
  STATIC_ASSERT_GT(inline_string<default_size>{"hello"}.compare(string_view{""}), 0);
}

/// @test `compare` returns less than zero when comparing an empty string against a non-empty string
TEST(ConstexprInlineStringCompare, AsEmptyToNonEmptyIsLess) {
  STATIC_ASSERT_LT(inline_string<default_size>{""}.compare(string_view{"hello"}), 0);
}

/// @test `compare` returns zero when comparing two empty strings
TEST(ConstexprInlineStringCompare, AsEmptyToEmptyIsEqual) {
  STATIC_ASSERT_EQ(inline_string<default_size>{""}.compare(string_view{""}), 0);
}

/// @test `compare` returns zero when comparing a string against itself
TEST(ConstexprInlineStringCompare, AgainstSelfIsEqual) {
  STATIC_ASSERT_EQ(inline_string<default_size>{"hello"}.compare(string_view{"hello"}), 0);
}

/// @test `compare` returns greater than zero when comparing a string against another which is a prefix
TEST(ConstexprInlineStringCompare, AgainstStrictPrefixIsGreater) {
  STATIC_ASSERT_GT(inline_string<default_size>{"hello"}.compare(string_view{"hell"}), 0);
}

/// @test `compare` returns less than zero when comparing a string against another which it is a prefix of
TEST(ConstexprInlineStringCompare, AgainstAsStrictPrefixIsLess) {
  STATIC_ASSERT_LT(inline_string<default_size>{"hell"}.compare(string_view{"hello"}), 0);
}

/// @test `compare` returns greater than zero if the string is lexicographically after the other string
TEST(ConstexprInlineStringCompare, AgainstLexicographicallyLessIsGreater) {
  STATIC_ASSERT_GT(inline_string<default_size>{"b"}.compare(string_view{"a"}), 0);
  STATIC_ASSERT_GT(inline_string<default_size>{"bat"}.compare(string_view{"bac"}), 0);
  STATIC_ASSERT_GT(inline_string<default_size>{"bat"}.compare(string_view{"bacz"}), 0);
  STATIC_ASSERT_GT(inline_string<default_size>{"bata"}.compare(string_view{"bac"}), 0);
}

/// @test `compare` returns less than zero if the string is lexicographically before the other string
TEST(ConstexprInlineStringCompare, AgainstLexicographicallyGreaterIsLess) {
  STATIC_ASSERT_LT(inline_string<default_size>{"a"}.compare(string_view{"b"}), 0);
  STATIC_ASSERT_LT(inline_string<default_size>{"bac"}.compare(string_view{"bat"}), 0);
  STATIC_ASSERT_LT(inline_string<default_size>{"bacz"}.compare(string_view{"bat"}), 0);
  STATIC_ASSERT_LT(inline_string<default_size>{"bac"}.compare(string_view{"bata"}), 0);
}

/// @test Invoking `compare` with an offset and size is equivalent to comparing the appropriate substring
TEST(ConstexprInlineStringCompare, WithOffsetAndSizeIsEquivalentToCompareAgainstSubstrWithOffsetAndSize) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, string_view{"b"}),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare(string_view{"b"})
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, string_view{"b"}),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare(string_view{"b"})
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, string_view{"bcd"}),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare(string_view{"bcd"})
  );
}

/// @test Invoking `compare` with an offset and size and `string_view` and position is equivalent to comparing the
/// appropriate substring against the substring of the view from the position
TEST(
    ConstexprInlineStringCompare,
    WithOffsetAndSizePlusOffsetIsEquivalentToCompareAgainstSubstrWithOffsetAndSizeAndSubstrWithOffset
) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, string_view{"b"}, 0U),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare(string_view{"b"}.substr(0U))
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, string_view{"b"}, 1U),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare(string_view{"b"}.substr(1U))
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, string_view{"bcd"}, 1U),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare(string_view{"bcd"}.substr(1U))
  );
}

/// @test Invoking `compare` with an offset and size and `string_view` and position and length is equivalent to
/// comparing the appropriate substring against the substring of the view from the position with the specified length
TEST(
    ConstexprInlineStringCompare,
    WithOffsetAndSizePlusOffsetAndSizeIsEquivalentToCompareAgainstSubstrWithOffsetAndSizeAndSubstrWithOffsetAndSize
) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, string_view{"b"}, 0U, 1U),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare(string_view{"b"}.substr(0U, 1U))
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, string_view{"b"}, 1U, 1U),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare(string_view{"b"}.substr(1U, 1U))
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, string_view{"bcd"}, 1U, 10U),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare(string_view{"bcd"}.substr(1U, 10U))
  );
}

/// @test Invoking `compare` with an offset and size and character pointer and length is equivalent to comparing the
/// appropriate substring against the substring of the character pointer with the length
TEST(
    ConstexprInlineStringCompare,
    WithOffsetAndSizePlusCStrAndSizeIsEquivalentToCompareAgainstSubstrWithOffsetAndSizeAndCStrWithSize
) {
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(0U, 1U, "b", 1U),
      inline_string<default_size>{"a"}.substr(0U, 1U).compare("b")
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"a"}.compare(1U, 1U, "b", 0U),
      inline_string<default_size>{"a"}.substr(1U, 1U).compare("")
  );
  STATIC_ASSERT_EQ(
      inline_string<default_size>{"abcd"}.compare(1U, 10U, "bcd", 2U),
      inline_string<default_size>{"abcd"}.substr(1U, 10U).compare("bc")
  );
}

/// @test `data` invoked on a `const` string returns a pointer to `const`
TEST(ConstexprInlineString, CanGetDataOnConstObject) {
  constexpr inline_string<default_size> inline_str{"hello world"};
  // NOLINTNEXTLINE(readability-container-data-pointer)
  STATIC_ASSERT_EQ(inline_str.data(), &inline_str[0]);
  ::testing::StaticAssertTypeEq<decltype(inline_str.data()), char const*>();
  STATIC_ASSERT_TRUE(noexcept(inline_str.data()));
}

/// @test `data` invoked on a non-`const` string returns a pointer to non-`const`
TEST(ConstexprInlineString, CanGetDataOnNonConstObject) {
  // NOLINTNEXTLINE(misc-const-correctness)
  inline_string<default_size> inline_str{"hello world"};
  STATIC_ASSERT_TRUE(noexcept(inline_str.data()));
  ::testing::StaticAssertTypeEq<decltype(inline_str.data()), char*>();
  // NOLINTNEXTLINE(bugprone-assignment-in-if-condition) Outside the macro this is in an assertion, not an if condition
  STATIC_ASSERT_EQ((*inline_string<default_size>{"hello"}.data() = 'a'), 'a');
}

/// @test An `inline_string` can be converted to a `null_terminated_string_view` with the pointer equal to that returned
/// from `c_str`
TEST(ConstexprInlineString, CanConvertToNullTerminatedString) {
  constexpr auto* original = "hello world";
  STATIC_ASSERT_STREQ(
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

}  // namespace noexcept_correctness_check

/// @test Copy and move are `noexcept`
TEST(ConstexprInlineStringForBugProneNoexceptClangTidy, NoexceptCopyAndMove) {
  inline_string<noexcept_correctness_check::size> dummy;
  dummy = noexcept_correctness_check::copy(dummy);
  noexcept_correctness_check::move(std::move(dummy));
}

// NOLINTEND(hicpp-move-const-arg, misc-const-correctness) This is explciitly testing the move behavior

/// @test `try_construct` returns an optional holding the desired string if the size fits in the capacity
TEST(ConstexprInlineStringTryConstruct, FromInlineStringReturnsStringIfSizeLessThanOrEqualCapacity) {
  {
    SCOPED_TRACE("Input with > capacity");
    STATIC_ASSERT_EQ(
        (inline_string<default_size>::try_construct(inline_string<default_size + 1>{"abc"})),
        inline_string<default_size>{"abc"}
    );
  }
  {
    SCOPED_TRACE("Input with <= capacity");
    STATIC_ASSERT_EQ(
        (inline_string<default_size>::try_construct(inline_string<default_size>{"abc"})),
        inline_string<default_size>{"abc"}
    );
  }
}

/// @test `try_construct` returns an optional holding the desired string if the size fits in the capacity when
/// passed a `string_view`
TEST(ConstexprInlineStringTryConstruct, FromStringViewReturnsStringIfSizeLessThanOrEqalCapacity) {
  STATIC_ASSERT_EQ(
      (inline_string<default_size>::try_construct(string_view{"abc"})),
      inline_string<default_size>{"abc"}
  );
}

}  // namespace
