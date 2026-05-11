// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/find.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/strings/algorithm/length.hpp"
#include "arene/base/strings/detail/string_length.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/inline_string_reference.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using ::arene::base::const_inline_string_reference;
using ::arene::base::inline_string;
using ::arene::base::inline_string_reference;
using ::arene::base::string_view;

// A simple default size to use in tests, shared with the tests for `inline_string` itself
constexpr std::size_t default_size = 123;

// const reference type
using const_ref_t = const_inline_string_reference;
// reference type
using ref_t = inline_string_reference;

/// @test Can make a `const_inline_string_reference` to an empty string and do basic operations through it
TEST(InlineStringRef, ConstRefOnEmptyString) {
  constexpr inline_string<default_size> const empty_str;

  STATIC_ASSERT_TRUE(const_ref_t(empty_str).empty());
  STATIC_ASSERT_EQ(const_ref_t(empty_str).size(), 0);
  STATIC_ASSERT_EQ(const_ref_t(empty_str).capacity(), default_size);
  STATIC_ASSERT_NE(const_ref_t(empty_str).c_str(), nullptr);
  STATIC_ASSERT_EQ(const_ref_t(empty_str).c_str(), empty_str.c_str());
  STATIC_ASSERT_EQ(const_ref_t(empty_str).data(), const_ref_t(empty_str).c_str());
}

/// @test Can make an `inline_string_reference` to an empty string and do basic operations through it
TEST(InlineStringRef, RefOnEmptyString) {
  inline_string<default_size> empty_str;

  ASSERT_TRUE(ref_t(empty_str).empty());
  ASSERT_EQ(ref_t(empty_str).size(), 0);
  ASSERT_EQ(ref_t(empty_str).capacity(), default_size);
  ASSERT_NE(ref_t(empty_str).c_str(), nullptr);
  ASSERT_EQ(ref_t(empty_str).c_str(), empty_str.c_str());
  ASSERT_EQ(ref_t(empty_str).data(), ref_t(empty_str).c_str());
}

/// @test Can make a `const_inline_string_reference` to a string and do basic operations through it
TEST(InlineStringRef, ConstRefOnNonEmptyString) {
  constexpr inline_string<default_size> const str("hello");

  STATIC_ASSERT_FALSE(const_ref_t(str).empty());
  ASSERT_EQ(const_ref_t(str).size(), arene::base::string_length("hello"));
  STATIC_ASSERT_EQ(const_ref_t(str).capacity(), default_size);
  STATIC_ASSERT_NE(const_ref_t(str).c_str(), nullptr);
  STATIC_ASSERT_EQ(*const_ref_t(str).c_str(), 'h');
  STATIC_ASSERT_STREQ(const_ref_t(str).c_str(), "hello");
  STATIC_ASSERT_EQ(const_ref_t(str).data(), const_ref_t(str).c_str());
  STATIC_ASSERT_EQ(const_ref_t(str).front(), 'h');
  STATIC_ASSERT_EQ(const_ref_t(str).back(), 'o');
}

/// @test Can make an `inline_string_reference` to a string and do basic operations through it
TEST(InlineStringRef, RefOnNonEmptyString) {
  inline_string<default_size> str("hello");

  ASSERT_FALSE(ref_t(str).empty());
  ASSERT_EQ(ref_t(str).size(), arene::base::string_length("hello"));
  ASSERT_EQ(ref_t(str).capacity(), default_size);
  ASSERT_NE(ref_t(str).c_str(), nullptr);
  ASSERT_EQ(*ref_t(str).c_str(), 'h');
  ASSERT_STREQ(ref_t(str).c_str(), "hello");
  ASSERT_EQ(ref_t(str).data(), const_ref_t(str).c_str());
  ASSERT_EQ(ref_t(str).front(), 'h');
  ASSERT_EQ(ref_t(str).back(), 'o');

  // conversion to const ref
  ASSERT_EQ(const_ref_t(ref_t(str)).front(), 'h');
}

constexpr std::size_t capacity = 42;
constexpr std::size_t capacity_other = 43;
constexpr std::size_t capacity_short = 6;

constexpr inline_string<capacity> str_const{"hello"};
constexpr inline_string<capacity_other> str_const_other{"world"};
constexpr inline_string<capacity_short> str_const_short{"hell"};

/// @test Can compare `inline_string_reference` and `const_inline_string_reference` instances for equality
TEST(InlineStringRefComparison, Equality) {
  constexpr std::size_t capacity_same = 5;
  constexpr inline_string<capacity_same> str_const_same{str_const};

  inline_string<capacity> str(str_const);
  inline_string<capacity_same> str_same(str_const_same);
  inline_string<capacity_other> str_other(str_const_other);

  // same type
  STATIC_ASSERT_EQ(const_ref_t(str_const), const_ref_t(str_const_same));
  STATIC_ASSERT_NE(const_ref_t(str_const_same), const_ref_t(str_const_other));
  EXPECT_EQ(ref_t(str), ref_t(str_same));
  EXPECT_NE(ref_t(str_same), ref_t(str_other));
  // cross types
  EXPECT_EQ(const_ref_t(str_const), ref_t(str));
  EXPECT_NE(const_ref_t(str_const), ref_t(str_other));
  EXPECT_EQ(ref_t(str), const_ref_t(str_const));
  EXPECT_NE(ref_t(str), const_ref_t(str_const_other));
}

/// @test Can compare `inline_string_reference` and `const_inline_string_reference` instances with less/less-equal
TEST(InlineStringRefComparison, LessThan) {
  inline_string<capacity> str(str_const);
  inline_string<capacity_other> str_other(str_const_other);
  inline_string<capacity_short> str_short(str_const_short);

  // same type
  STATIC_ASSERT_LT(const_ref_t(str_const), const_ref_t(str_const_other));
  STATIC_ASSERT_LT(const_ref_t(str_const_short), const_ref_t(str_const));
  STATIC_ASSERT_LE(const_ref_t(str_const), const_ref_t(str_const_other));
  STATIC_ASSERT_LE(const_ref_t(str_const_short), const_ref_t(str_const));
  ASSERT_LT(ref_t(str), ref_t(str_other));
  ASSERT_LT(ref_t(str_short), ref_t(str));
  ASSERT_LE(ref_t(str), ref_t(str_other));
  ASSERT_LE(ref_t(str_short), ref_t(str));
  // cross types
  ASSERT_LT(const_ref_t(str_const), ref_t(str_other));
  ASSERT_LT(const_ref_t(str_const_short), ref_t(str));
  ASSERT_LE(const_ref_t(str_const), ref_t(str_other));
  ASSERT_LE(const_ref_t(str_const_short), ref_t(str));
  ASSERT_LT(ref_t(str), const_ref_t(str_const_other));
  ASSERT_LT(ref_t(str_short), const_ref_t(str_const));
  ASSERT_LE(ref_t(str), const_ref_t(str_const_other));
  ASSERT_LE(ref_t(str_short), const_ref_t(str_const));
}

/// @test Can compare `inline_string_reference` and `const_inline_string_reference` instances with greater/greater-equal
TEST(InlineStringRefComparison, GreaterThan) {
  inline_string<capacity> str(str_const);
  inline_string<capacity_other> str_other(str_const_other);
  inline_string<capacity_short> str_short(str_const_short);

  // same type
  STATIC_ASSERT_GT(const_ref_t(str_const_other), const_ref_t(str_const));
  STATIC_ASSERT_GT(const_ref_t(str_const), const_ref_t(str_const_short));
  STATIC_ASSERT_GE(const_ref_t(str_const_other), const_ref_t(str_const));
  STATIC_ASSERT_GE(const_ref_t(str_const), const_ref_t(str_const_short));
  ASSERT_GT(ref_t(str_other), ref_t(str));
  ASSERT_GT(ref_t(str), ref_t(str_short));
  ASSERT_GE(ref_t(str_other), ref_t(str));
  ASSERT_GE(ref_t(str), ref_t(str_short));
  // cross types
  ASSERT_GT(ref_t(str_other), const_ref_t(str_const));
  ASSERT_GT(ref_t(str), const_ref_t(str_const_short));
  ASSERT_GE(ref_t(str_other), const_ref_t(str_const));
  ASSERT_GE(ref_t(str), const_ref_t(str_const_short));
  ASSERT_GT(const_ref_t(str_const_other), ref_t(str));
  ASSERT_GT(const_ref_t(str_const), ref_t(str_short));
  ASSERT_GE(const_ref_t(str_const_other), ref_t(str));
  ASSERT_GE(const_ref_t(str_const), ref_t(str_short));
}

/// @test Can compare `const_inline_string_reference` instances to various other types for equality
TEST(InlineStringRefComparison, OtherTypeEquality) {
  constexpr const_ref_t const_ref(str_const);
  constexpr const_ref_t const_ref_other(str_const_other);
  char const* const c_string_short(str_const_short.c_str());

  EXPECT_NE(const_ref_other, c_string_short);

  STATIC_ASSERT_TRUE(const_ref == "hello");
  STATIC_ASSERT_TRUE(const_ref != "world");

  STATIC_ASSERT_EQ(const_ref_other, "world");
  STATIC_ASSERT_NE(const_ref_other, "hello");
  STATIC_ASSERT_LT(const_ref_other, "worst");
  STATIC_ASSERT_LE(const_ref_other, "worst");
  STATIC_ASSERT_GT(const_ref_other, "hello");
  STATIC_ASSERT_GE(const_ref_other, "hello");
}

/// @test Can compare `const_inline_string_reference` instances to various other types for ordering
TEST(InlineStringRefComparison, OtherTypeOrdering) {
  constexpr const_ref_t const_ref(str_const);
  constexpr const_ref_t const_ref_other(str_const_other);
  char const* const c_string_short(str_const_short.c_str());

  ASSERT_LT(c_string_short, const_ref);
  ASSERT_LE(c_string_short, const_ref);

  ASSERT_GT(const_ref_other, c_string_short);
  ASSERT_GE(const_ref_other, c_string_short);
}

/// @test Can compare `inline_string_reference` instances to various other types
TEST(InlineStringRefComparison, MutableComparison) {
  inline_string<default_size> str(str_const);
  ref_t const ref(str);

  EXPECT_TRUE(ref == "hello");
  EXPECT_TRUE(ref != "goodbye");
}

/// @test Can access random characters in a `const_inline_string_reference`
TEST(InlineStringRef, RandomAccessConst) {
  constexpr inline_string<default_size> str("hello");

  const_ref_t::size_type index = 0;
  for (auto it = const_ref_t(str).begin(); it != const_ref_t(str).end(); ++it) {
    ASSERT_EQ(*it, const_ref_t(str)[index]);
    ASSERT_EQ(*it, const_ref_t(str).begin()[static_cast<std::ptrdiff_t>(index++)]);
  }
}

/// @test Can access random characters in an `inline_string_reference`
TEST(InlineStringRef, RandomAccess) {
  inline_string<default_size> str("hello");

  const_ref_t::size_type index = 0;
  for (auto it = ref_t(str).begin(); it != ref_t(str).end(); ++it) {
    ASSERT_EQ(*it, ref_t(str)[index]);
    ASSERT_EQ(*it, ref_t(str).begin()[static_cast<std::ptrdiff_t>(index++)]);
  }

  // edit string
  (ref_t(str))[0] = 'j';
  ASSERT_STREQ(ref_t(str).c_str(), "jello");
  (ref_t(str))[str.size() - 1] = 'y';
  ASSERT_STREQ(ref_t(str).c_str(), "jelly");
  ref_t(str).front() = 'h';
  ref_t(str).back() = 'o';
  ASSERT_STREQ(ref_t(str).c_str(), "hello");
}

/// @brief Constexpr wrapper for the @c append operation
/// @tparam Rhs The type(s) of the argument(s) to pass to the underlying @c append function
/// @param rhs The argument(s) to pass to the underlying @c append function
/// @return A copy of the input string that has had `append(args...)` called on it.
template <typename... Rhs>
constexpr auto do_append(inline_string<default_size> lhs, Rhs&&... rhs) -> inline_string<default_size> {
  // NOLINTNEXTLINE(hicpp-no-array-decay) If Rhs is an array this causes a pointer decay but it's just a test
  ref_t(lhs).append(std::forward<Rhs>(rhs)...);
  return lhs;
}

/// @test Can append through an `inline_string_reference` from a string literal
TEST(InlineStringRefAppend, StringLiteralAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", " world"), "hello world");
}

/// @test Can append through an `inline_string_reference` from a C-style character pointer
TEST(InlineStringRefAppend, CStringAtEndOfString) {
  constexpr auto* world = " world";
  STATIC_ASSERT_EQ(do_append("hello", world), "hello world");
}

/// @test Can append through an `inline_string_reference` from a `string_view`
TEST(InlineStringRefAppend, StringViewAtEndOfString) {
  STATIC_ASSERT_EQ(do_append("hello", string_view{" world"}), "hello world");
}

/// @test Can append through an `inline_string_reference` from a count and a character
TEST(InlineStringRefAppend, CharRepeatingAtEndOfString) { STATIC_ASSERT_EQ(do_append("hello", 5U, 'x'), "helloxxxxx"); }

/// @brief Constexpr wrapper for += operation
/// @tparam Rhs The type of the argument to pass
/// @return A copy of the input string that has had `+= rhs` called on it.
template <typename Rhs>
constexpr auto do_plus_assign(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  ref_t(lhs) += rhs;
  return lhs;
}

/// @test Can append through an `inline_string_reference` from a string literal using `operator+=`
TEST(InlineStringRefPlusAssign, StringLiteralAtEndOfString) {
  STATIC_ASSERT_EQ(do_plus_assign("hello", " world"), "hello world");
}

/// @test Can append through an `inline_string_reference` from a C-style string using `operator+=`
TEST(InlineStringRefPlusAssign, CStringAtEndOfString) {
  constexpr auto* world = " world";
  STATIC_ASSERT_EQ(do_plus_assign("hello", world), "hello world");
}

/// @test Can append through an `inline_string_reference` from a single character using `operator+=`
TEST(InlineStringRefPlusAssign, CharAtEndOfString) { STATIC_ASSERT_EQ(do_plus_assign("hello", 'x'), "hellox"); }

/// @brief Constexpr wrapper for push_back operation
/// @tparam Rhs The type of the argument to push back
/// @return The input string with push_back(arg) called on it.
template <typename Rhs>
constexpr auto do_push_back(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  ref_t(lhs).push_back(rhs);
  return lhs;
}

/// @test Can append through an `inline_string_reference` from a single character
TEST(InlineStringRefPushBack, AppendsCharToEndOfString) { STATIC_ASSERT_EQ(do_push_back("hello", 'Z'), "helloZ"); }

/// @test Can append through an `inline_string_reference` from a C-style string
TEST(InlineStringRefPushBack, AppendsStringToEndOfString) {
  constexpr auto* str = "ZZZ";
  STATIC_ASSERT_EQ(do_push_back("hello", str), "helloZZZ");
}

/// @brief Constexpr wrapper for the @c assign function
/// @tparam Rhs The type of the argument to pass
/// @param rhs The argument to pass
/// @return A copy of the input string that has had `.assign(rhs)` called on it.
template <typename Rhs>
constexpr auto do_assign(inline_string<default_size> lhs, Rhs&& rhs) -> inline_string<default_size> {
  ref_t(lhs).assign(std::forward<Rhs>(rhs));
  return lhs;
}

/// @test Can assign through an `inline_string_reference` using an `inline_string`
TEST(InlineStringRefAssignment, Assignment) {
  constexpr inline_string<default_size> const str{"hello"};
  constexpr inline_string<default_size> str_2;
  constexpr inline_string<default_size> str_3 = do_assign(str_2, str);
  STATIC_ASSERT_EQ(str_3, str);
  STATIC_ASSERT_STREQ(const_ref_t(str_3).c_str(), str.c_str());
}

/// @test Can assign through an `inline_string_reference` using a C-style character pointer
TEST(InlineStringRefAssignment, AssignmentFromCharPointer) {
  constexpr char const* c_string{"hello"};
  constexpr inline_string<default_size> inline_str;
  constexpr inline_string<default_size> assigned_str = do_assign(inline_str, c_string);
  STATIC_ASSERT_EQ(assigned_str, "hello");
  STATIC_ASSERT_STREQ(const_ref_t(assigned_str).c_str(), c_string);
}

/// @test Can assign through an `inline_string_reference` using a `string_view`
TEST(InlineStringRefAssignment, AssignmentFromStringViewWithOffset) {
  string_view const view{"hello"};
  inline_string<default_size> inline_str;
  auto ref = ref_t(inline_str);
  ref.assign(view.substr(2));
  ASSERT_EQ(inline_str, "llo");
  ASSERT_EQ(ref.c_str(), inline_str);
}

/// @brief Constexpr wrapper for the @c erase function
/// @tparam Rhs The type(s) of the argument(s) to pass
/// @param rhs The argument(s) to pass
/// @return A copy of the input string that has had `.erase(rhs)` called on it.
template <typename... Rhs>
constexpr auto do_erase(inline_string<default_size> lhs, Rhs&&... rhs) -> inline_string<default_size> {
  ref_t(lhs).erase(std::forward<Rhs>(rhs)...);
  return lhs;
}

/// @test Can erase from inside an `inline_string_reference` using a position and length at compile time
TEST(InlineStringRefErase, ConstexprEraseWithPositionAndLength) {
  constexpr inline_string<default_size> inline_str{"abcdefg"};
  constexpr inline_string<default_size> erased_str = do_erase(inline_str, 3U, 2U);
  STATIC_ASSERT_EQ(const_ref_t(erased_str).capacity(), erased_str.capacity());
  STATIC_ASSERT_EQ(const_ref_t(erased_str).data(), erased_str);
  STATIC_ASSERT_EQ(erased_str, "abcfg");
  STATIC_ASSERT_STREQ(erased_str.c_str(), "abcfg");
}

/// @test Can erase from inside an `inline_string_reference` using a position and length at runtime
TEST(InlineStringRefErase, RuntimeEraseWithPositionAndLength) {
  inline_string<default_size> inline_str{"abcdefg"};
  ref_t(inline_str).erase(3U, 2U);
  ASSERT_EQ(inline_str, "abcfg");
  ASSERT_STREQ(inline_str.c_str(), "abcfg");
}

/// @test Can erase from inside an `inline_string_reference` using an iterator range
TEST(InlineStringRefErase, EraseWithIteratorRange) {
  inline_string<default_size> inline_str{"abcdefghijkl"};
  auto const pos = inline_str.begin() + 2;
  auto const end = pos + 3;
  inline_string<default_size>::iterator const res = ref_t(inline_str).erase(pos, end);
  ASSERT_EQ(res, pos);
  ASSERT_EQ(ref_t(inline_str), "abfghijkl");
}

/// @test Can use ADL `find` to find characters inside of an `const_inline_string_reference`
TEST(InlineStringRefAlgo, Find) {
  constexpr inline_string<default_size> str("hello");
  STATIC_ASSERT_FALSE(find(const_ref_t(str).begin(), const_ref_t(str).end(), 'l') == const_ref_t(str).end());
  STATIC_ASSERT_TRUE(find(const_ref_t(str).begin(), const_ref_t(str).end(), 'L') == const_ref_t(str).end());
  STATIC_ASSERT_TRUE(*find(const_ref_t(str).begin(), const_ref_t(str).end(), 'o') == const_ref_t(str).back());
  STATIC_ASSERT_TRUE(*find(const_ref_t(str).begin(), const_ref_t(str).end(), 'h') == const_ref_t(str).front());
}

/// @test Member functions of `const_inline_string_reference` have the correct `noexcept` specification
TEST(InlineStringRef, NoexceptConstMembers) {
  constexpr inline_string<default_size> str;
  STATIC_ASSERT_TRUE(noexcept(const_ref_t(str)));
  auto ref = const_ref_t(str);
  STATIC_ASSERT_TRUE(noexcept(const_ref_t(ref)));

  STATIC_ASSERT_TRUE(noexcept(ref.c_str()));
  STATIC_ASSERT_TRUE(noexcept(ref.data()));
  STATIC_ASSERT_TRUE(noexcept(ref.size()));
  STATIC_ASSERT_TRUE(noexcept(ref.capacity()));
  STATIC_ASSERT_TRUE(noexcept(ref.empty()));

  STATIC_ASSERT_TRUE(noexcept(ref[0]));
  STATIC_ASSERT_TRUE(noexcept(ref.front()));
  STATIC_ASSERT_TRUE(noexcept(ref.back()));
  STATIC_ASSERT_TRUE(noexcept(ref.begin()));
  STATIC_ASSERT_TRUE(noexcept(ref.end()));

  STATIC_ASSERT_TRUE(noexcept(ref.to_string_view()));
}

/// @test Member functions of `inline_string_reference` have the correct `noexcept` specification
TEST(InlineStringRef, NoexceptMutableMembers) {
  inline_string<default_size> str;
  STATIC_ASSERT_TRUE(noexcept(ref_t(str)));
  auto ref = ref_t(str);
  STATIC_ASSERT_TRUE(noexcept(ref_t(ref)));

  STATIC_ASSERT_TRUE(noexcept(ref.c_str()));
  STATIC_ASSERT_TRUE(noexcept(ref.data()));
  STATIC_ASSERT_TRUE(noexcept(ref.size()));
  STATIC_ASSERT_TRUE(noexcept(ref.capacity()));
  STATIC_ASSERT_TRUE(noexcept(ref.empty()));

  STATIC_ASSERT_TRUE(noexcept(ref[0]));
  STATIC_ASSERT_TRUE(noexcept(ref.front()));
  STATIC_ASSERT_TRUE(noexcept(ref.back()));
  STATIC_ASSERT_TRUE(noexcept(ref.begin()));
  STATIC_ASSERT_TRUE(noexcept(ref.end()));

  STATIC_ASSERT_TRUE(noexcept(ref.to_string_view()));

  STATIC_ASSERT_TRUE(noexcept(ref.assign(str)));
  STATIC_ASSERT_TRUE(noexcept(ref.append(str)));
  STATIC_ASSERT_TRUE(noexcept(ref.append(0, 'a')));
  static_assert(noexcept(ref += str), "Must be noexcept");  // clang-tidy doesn't like these being in the minitest macro
  static_assert(noexcept(ref += 'a'), "Must be noexcept");  // clang-tidy doesn't like these being in the minitest macro
  STATIC_ASSERT_TRUE(noexcept(ref.push_back(str)));
  STATIC_ASSERT_TRUE(noexcept(ref.push_back('a')));
  STATIC_ASSERT_TRUE(noexcept(ref.erase(0, 0)));
  STATIC_ASSERT_TRUE(noexcept(ref.erase(ref.begin(), ref.begin())));
}

/// @test Comparisons of inline string references are all noexcept
TEST(InlineStringRef, NoexceptComparisons) {
  inline_string<default_size> str;
  auto const_ref = const_ref_t(str);
  auto ref = ref_t(str);

  STATIC_ASSERT_TRUE(noexcept(const_ref == const_ref));
  STATIC_ASSERT_TRUE(noexcept(const_ref == ref));
  STATIC_ASSERT_TRUE(noexcept(ref == const_ref));
  STATIC_ASSERT_TRUE(noexcept(ref == ref));

  STATIC_ASSERT_TRUE(noexcept(const_ref != const_ref));
  STATIC_ASSERT_TRUE(noexcept(const_ref != ref));
  STATIC_ASSERT_TRUE(noexcept(ref != const_ref));
  STATIC_ASSERT_TRUE(noexcept(ref != ref));

  STATIC_ASSERT_TRUE(noexcept(const_ref < const_ref));
  STATIC_ASSERT_TRUE(noexcept(const_ref < ref));
  STATIC_ASSERT_TRUE(noexcept(ref < const_ref));
  STATIC_ASSERT_TRUE(noexcept(ref < ref));

  STATIC_ASSERT_TRUE(noexcept(const_ref <= const_ref));
  STATIC_ASSERT_TRUE(noexcept(const_ref <= ref));
  STATIC_ASSERT_TRUE(noexcept(ref <= const_ref));
  STATIC_ASSERT_TRUE(noexcept(ref <= ref));

  STATIC_ASSERT_TRUE(noexcept(const_ref > const_ref));
  STATIC_ASSERT_TRUE(noexcept(const_ref > ref));
  STATIC_ASSERT_TRUE(noexcept(ref > const_ref));
  STATIC_ASSERT_TRUE(noexcept(ref > ref));

  STATIC_ASSERT_TRUE(noexcept(const_ref >= const_ref));
  STATIC_ASSERT_TRUE(noexcept(const_ref >= ref));
  STATIC_ASSERT_TRUE(noexcept(ref >= const_ref));
  STATIC_ASSERT_TRUE(noexcept(ref >= ref));
}

/// @test member functions of @c inline_string_reference are invocable with a @c const qualified @c
/// inline_string_reference
TEST(InlineStringRef, MemberFunctionsAreConstInvocable) {
  using arene::base::inline_string_reference;
  using arene::base::string_view;
  using size_type = typename inline_string_reference::size_type;
  using const_iterator = typename inline_string_reference::const_iterator;

  using arene::base::is_invocable_v;

  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::c_str), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::data), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::size), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::capacity), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::empty), inline_string_reference const&>));
  STATIC_ASSERT_TRUE(
      (is_invocable_v<decltype(&inline_string_reference::operator[]), inline_string_reference const&, size_type>)
  );
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::front), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::back), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::begin), inline_string_reference const&>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(&inline_string_reference::end), inline_string_reference const&>));
  STATIC_ASSERT_TRUE(
      (is_invocable_v<decltype(&inline_string_reference::to_string_view), inline_string_reference const&>)
  );
  STATIC_ASSERT_TRUE(
      (is_invocable_v<decltype(&inline_string_reference::assign), inline_string_reference const&, string_view>)
  );

  auto const append = [](auto const& ref, auto... args) { return ref.append(args...); };
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(append), inline_string_reference const&, string_view>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(append), inline_string_reference const&, size_type, char>));

  auto const compound_plus = [](auto const& ref, auto arg) { return ref += arg; };
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(compound_plus), inline_string_reference const&, string_view>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(compound_plus), inline_string_reference const&, char>));

  auto const push_back = [](auto const& ref, auto arg) { return ref.push_back(arg); };
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(push_back), inline_string_reference const&, string_view>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(push_back), inline_string_reference const&, char>));

  auto const erase = [](auto const& ref, auto... args) { return ref.erase(args...); };
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(erase), inline_string_reference const&, size_type, size_type>));
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(erase), inline_string_reference const&, const_iterator, const_iterator>));
}

template <typename InlineStringRefT>
using use_at = decltype(std::declval<InlineStringRefT>().at(0));

template <typename InlineStringRefT>
constexpr bool has_at_v = arene::base::substitution_succeeds<use_at, InlineStringRefT>;

/// @test Given an instance of `inline_string_reference`, then `inline_string_reference::at()`'s invocability matches
/// `arene::base::are_exceptions_enabled_v`.
TEST(InlineStringRef, AtInvocabilityMatchesAreExceptionsEnabled) {
  STATIC_ASSERT_EQ(has_at_v<ref_t&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<ref_t const&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<ref_t&&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<ref_t const&&>, arene::base::detail::are_exceptions_enabled_v);

  STATIC_ASSERT_EQ(has_at_v<const_ref_t&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<const_ref_t const&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<const_ref_t&&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<const_ref_t const&&>, arene::base::detail::are_exceptions_enabled_v);
}

}  // namespace
