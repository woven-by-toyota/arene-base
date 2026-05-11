// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/string_view.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/strings/detail/string_length.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <sstream>
#endif

namespace {

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace arene::base::literals;

using ::arene::base::array;
using ::arene::base::consume_values;
using ::arene::base::dynamic_extent;
using ::arene::base::next;
using ::arene::base::span;
using ::arene::base::string_view;
using ::arene::base::detail::string_length;

constexpr std::size_t max_string_length{256};

/// @test Simple checks for `string_view` behaviour:
/// - a default-constructed `string_view` is empty
/// - constructing with a pointer to a NUL-terminated string yields a `string_view` with the full length of the string
/// - constructing with a pointer and a size has the specified size
TEST(StringView, StringViewSimpleCheck) {
  constexpr auto* msg_literal_constchar = u8"こんにちは Arene OS";

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  constexpr string_view full(msg_literal_constchar);

  constexpr std::size_t computed_len = string_length(full.data());

  STATIC_ASSERT_EQ(computed_len, full.size());

  constexpr string_view empty;

  constexpr std::size_t truncated_size = 5;
  constexpr string_view truncated(static_cast<char const*>(msg_literal_constchar), truncated_size);

  STATIC_ASSERT_TRUE(empty.empty());
  STATIC_ASSERT_FALSE(truncated.empty());
  STATIC_ASSERT_FALSE(full.empty());

  STATIC_ASSERT_EQ(empty.size(), 0);
  STATIC_ASSERT_EQ(empty.size(), empty.length());
  STATIC_ASSERT_EQ(empty.data(), nullptr);

  STATIC_ASSERT_EQ(truncated.size(), truncated_size);
  STATIC_ASSERT_EQ(truncated.size(), truncated.length());

  string_view const new_view_copied = full;
  string_view truncated_copy = truncated;

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  string_view const new_view_moved = std::move(truncated_copy);

  EXPECT_EQ(full, new_view_copied);
  EXPECT_EQ(new_view_moved, truncated);
}

/// @test The @c string_view stream operator yields the same content as the @c string_view
TEST(StringView, StringViewStreamOperator) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  constexpr auto* msg_literal_constchar = u8"こんにちは Arene OS";
  constexpr string_view strview(msg_literal_constchar);

  std::ostringstream stream;
  stream << strview;
  EXPECT_EQ(stream.str(), arene::base::to_string(strview));
#else
  GTEST_SKIP() << "No stream operator when using Arene Base stdlib implementation";
#endif
}

/// @test Invoking `compare` on the result of `substr` yields results consistent with std::string lexicographic ordering
TEST(StringView, StringViewCompareSubstr) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto const& msg = u8"hello arene #42~€é";

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  constexpr string_view msg_view(msg);
  std::string const msg_str(static_cast<char const*>(msg));

  string_view const msg_view_str(msg_str.data(), msg_str.size());

  std::string const sub_str = msg_str.substr(0, 6);
  EXPECT_EQ(msg_view.substr(0, 6).compare(string_view(sub_str.data(), sub_str.size())), 0);

  EXPECT_EQ(msg_view.compare(msg_view_str), 0);
  EXPECT_EQ(msg_view.compare(msg_view.substr(0, 1024 /* way larger */)), 0);
  EXPECT_EQ(msg_view.substr(0, 5).compare(msg_view), -1);
  EXPECT_EQ(msg_view.compare(msg_view.substr(0, 7)), 1);
#else
  GTEST_SKIP() << "No std::string when using Arene Base stdlib implementation";
#endif
}

/// @test Using the `_asv` literal suffix yields a `string_view` referring to the appropriate characters
TEST(StringView, StringliteralTest) {
  auto const str_view = "hello world"_asv;

  ::testing::StaticAssertTypeEq<decltype(str_view), string_view const>();
  EXPECT_EQ(str_view, string_view{"hello world"});
}

/// @test The comparison operators yield results consistent with lexicographic ordering
TEST(StringView, StringOperatorCheck) {
  constexpr string_view empty;
  constexpr string_view str_viewh = "hello arene";
  constexpr string_view str_viewb = "Arene OS";
  constexpr string_view str_viewh_copy = str_viewh;

  STATIC_ASSERT_EQ(str_viewh, str_viewh_copy);
  STATIC_ASSERT_GE(str_viewh, str_viewh_copy);
  STATIC_ASSERT_LE(str_viewh, str_viewh_copy);

  STATIC_ASSERT_NE(str_viewh, str_viewb);

  STATIC_ASSERT_GT(str_viewh, empty);
  STATIC_ASSERT_GE(str_viewh, empty);
  STATIC_ASSERT_LT(str_viewb, str_viewh);
  STATIC_ASSERT_LE(str_viewb, str_viewh);
}

/// @test Constructing a `string_view` from a `std::string` yields a view with the result of `data` equal to the result
/// of `c_str` on the `std::string`, and the result of `size` equal to the result of `size` on the `std::string`.
TEST(StringView, ConvertibleFromStdString) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  std::string const k_hello = "hello world";
  string_view const str_view = k_hello;
  EXPECT_EQ(str_view.data(), k_hello.c_str());
  EXPECT_EQ(str_view.size(), k_hello.size());
#else
  GTEST_SKIP() << "No std::string when using Arene Base stdlib implementation";
#endif
}

/// @test A `string_view` cannot be constructed from an initializer list
TEST(StringView, CannotConstructFromInitializerList) {
  STATIC_ASSERT_FALSE(std::is_constructible<string_view, std::initializer_list<char>>::value);
}

/// @test The indexing operator can be used to read the characters from a `string_view`
TEST(StringView, Indexing) {
  string_view const str_view{"hello"};
  ASSERT_EQ(str_view[0], 'h');
  ASSERT_EQ(str_view[1], 'e');
  ASSERT_EQ(str_view[2], 'l');
  ASSERT_EQ(str_view[3], 'l');
  ASSERT_EQ(str_view[4], 'o');
}

/// @test Invoking `front` returns a reference to the first character in the string
TEST(StringView, FrontGetsFirstCharacter) {
  constexpr string_view str_view{"hello"};
  STATIC_ASSERT_EQ(str_view.front(), 'h');
  ::testing::StaticAssertTypeEq<decltype(str_view.front()), char const&>();
  // NOLINTNEXTLINE(readability-container-data-pointer)
  STATIC_ASSERT_EQ(&str_view.front(), &str_view[0]);
}

/// @test Invoking `back` returns a reference to the last character in the string
TEST(StringView, BackGetsLastCharacter) {
  constexpr string_view str_view{"hello"};
  STATIC_ASSERT_EQ(str_view.back(), 'o');
  ::testing::StaticAssertTypeEq<decltype(str_view.back()), char const&>();
  STATIC_ASSERT_EQ(&str_view.back(), &str_view[str_view.size() - 1]);
}

namespace {
constexpr auto do_remove_prefix(string_view str_view, std::size_t count) -> string_view {
  str_view.remove_prefix(count);
  return str_view;
}
constexpr auto do_remove_suffix(string_view str_view, std::size_t count) -> string_view {
  str_view.remove_suffix(count);
  return str_view;
}
}  // namespace

/// @test `remove_prefix` returns a `string_view` referring to the tail end of the original string, starting at the
/// specified offset.
TEST(StringViewRemovePrefix, RemovesFirstCountCharacters) {
  constexpr string_view str_view{"hello"};
  STATIC_ASSERT_EQ(do_remove_prefix(str_view, 3), "lo");
  STATIC_ASSERT_EQ(do_remove_prefix(str_view, 2), "llo");
  STATIC_ASSERT_EQ(do_remove_prefix(str_view, 2).data(), &str_view[2]);
}

/// @test `remove_prefix` returns an empty `string_view` if the length to remove is greater than or equal to the length
/// of the string.
TEST(StringViewRemovePrefix, IfCountGESizeResultsInEmptyStringView) {
  constexpr string_view str_view{"hello"};
  STATIC_ASSERT_TRUE(do_remove_prefix(str_view, str_view.size()).empty());
  STATIC_ASSERT_TRUE(do_remove_prefix(str_view, str_view.size() + 10).empty());
}

/// @test `remove_suffix` returns a `string_view` with the same `data`, but the length reduced by the specified count
TEST(StringViewRemoveSuffix, RemovesLastCountCharacters) {
  constexpr string_view str_view{"hello"};
  ASSERT_EQ(do_remove_suffix(str_view, 3), "he");
  ASSERT_EQ(do_remove_suffix(str_view, 2), "hel");
  // NOLINTNEXTLINE(readability-container-size-empty)
  ASSERT_EQ(do_remove_suffix(str_view, 5), "");
  ASSERT_EQ(do_remove_suffix(str_view, 2).data(), str_view.data());
  ASSERT_EQ(do_remove_suffix(str_view, 5).data(), str_view.data());
}

/// @test `remove_suffix` returns an empty `string_view` if the length to remove is greater than or equal to the length
/// of the string.
TEST(StringViewRemoveSuffix, IfCountGESizeResultsInEmptyStringView) {
  constexpr string_view str_view = "hello";
  STATIC_ASSERT_TRUE(do_remove_suffix(str_view, str_view.size()).empty());
  STATIC_ASSERT_TRUE(do_remove_suffix(str_view, str_view.size() + 10).empty());
}

static_assert(string_view::npos == std::numeric_limits<std::size_t>::max(), "npos is max size_type value");

/// @test Invoking `substr` with no arguments returns a copy of the original `string_view`
TEST(StringViewSubstr, WithNoArgsIsWholeString) {
  constexpr string_view str_view{"hello"};
  static_assert(str_view.substr().data() == str_view.data(), "Same start");
  static_assert(str_view.substr().length() == str_view.length(), "Same length");

  auto const str_view2 = str_view.substr();
  ASSERT_EQ(str_view2.data(), str_view.data());
  ASSERT_EQ(str_view2.length(), str_view.length());
}

/// @test Invoking `substr` with a position returns a `string_view` with the `data` equal to the address of the
/// character at the specified position in the original `string_view`, and a length equal to the distance from that
/// position to the end of the string
TEST(StringViewSubstr, WithPosIsSubstringFromThatOffset) {
  constexpr string_view str_view{"hello"};
  constexpr auto offset = 3;
  STATIC_ASSERT_EQ(str_view.substr(offset).data(), next(str_view.data(), offset));
  STATIC_ASSERT_EQ(str_view.substr(offset).length(), (str_view.length() - offset));

  constexpr auto offset2 = 1;
  STATIC_ASSERT_EQ(str_view.substr(offset2).data(), next(str_view.data(), offset2));
  STATIC_ASSERT_EQ(str_view.substr(offset2).length(), (str_view.length() - offset2));
  STATIC_ASSERT_EQ(str_view.substr(str_view.length()).data(), next(str_view.data(), str_view.length()));
  STATIC_ASSERT_EQ(str_view.substr(str_view.length()).length(), 0);
}

/// @test Invoking `substr` with a position and a length returns a `string_view` with the `data` equal to the address of
/// the character at the specified position in the original `string_view`, and the specified length
TEST(StringViewSubstr, WithPosAndLengthIsSubstringWithOffsetAndLength) {
  constexpr string_view str_view{"hello world"};
  constexpr auto offset = 3;
  constexpr auto length = 4;
  STATIC_ASSERT_EQ(str_view.substr(offset, length).data(), next(str_view.data(), offset));
  STATIC_ASSERT_EQ(str_view.substr(offset, length).length(), length);
}

/// @test Invoking `substr` with a position and a length where the length is longer than the remaining characters from
/// the specified position returns a `string_view` with the `data` equal to the address of the character at the
/// specified position in the original `string_view`, and a length equal to the distance from that position to the end
/// of the string
TEST(StringViewSubstr, WithPosAndLengthIsSubstringFromOffsetIfLengthTooLong) {
  constexpr string_view str_view{"hello world"};
  constexpr auto offset = 3;
  constexpr auto length = string_view::npos;
  STATIC_ASSERT_EQ(str_view.substr(offset, length).data(), next(str_view.data(), offset));
  STATIC_ASSERT_EQ(str_view.substr(offset, length).length(), str_view.length() - offset);
}

/// @test Invoking `substr` with a position, with or without a length, that is greater than or equal to the length of
/// the `string_view` returns an empty `string_view`.
TEST(StringViewSubstr, WithPosIsEmptyStringIfGESize) {
  constexpr string_view str_view{"hello"};
  STATIC_ASSERT_TRUE(str_view.substr(str_view.size()).empty());
  STATIC_ASSERT_TRUE(str_view.substr(str_view.size() + 10).empty());
  STATIC_ASSERT_TRUE(str_view.substr(str_view.size(), 1).empty());
  STATIC_ASSERT_TRUE(str_view.substr(str_view.size() + 10, 1).empty());
}

namespace {
struct implicit_conversion_wrapper {
  string_view str_view;

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator char const*() const noexcept { return str_view.data(); }
};
}  // namespace

/// @test `starts_with` returns `true` if and only if the length of the `string_view` is greater than or equal to the
/// length of the prefix, and the initial characters of the `string_view` are equal to the prefix.
TEST(StringView, StartsWith) {
  constexpr string_view str_view{"hello"};
  constexpr string_view str_view2{"hell"};
  constexpr string_view str_view3{"goodbye"};

  STATIC_ASSERT_TRUE(str_view.starts_with(str_view));
  STATIC_ASSERT_TRUE(str_view.starts_with(str_view2));
  STATIC_ASSERT_FALSE(str_view.starts_with(str_view3));
  STATIC_ASSERT_FALSE(str_view.starts_with(str_view3.data()));
  STATIC_ASSERT_TRUE(str_view.starts_with(str_view2.data()));
  STATIC_ASSERT_FALSE(str_view.starts_with("hello world"));

  STATIC_ASSERT_TRUE(str_view.starts_with(implicit_conversion_wrapper{str_view}));

  STATIC_ASSERT_TRUE(str_view.starts_with('h'));
  STATIC_ASSERT_FALSE(str_view.starts_with('x'));
  STATIC_ASSERT_FALSE(string_view{}.starts_with('x'));

  STATIC_ASSERT_TRUE(noexcept(str_view.starts_with('h')));
  STATIC_ASSERT_TRUE(noexcept(str_view.starts_with("h")));
  STATIC_ASSERT_TRUE(noexcept(str_view.starts_with(str_view)));
  string_view const str_view4{"hello world"};
  ASSERT_TRUE(str_view4.starts_with(str_view));
  ASSERT_FALSE(str_view4.starts_with("abc"));
  ASSERT_TRUE(str_view4.starts_with('h'));

  string_view const truncated = str_view4.substr(0, 3);
  ASSERT_FALSE(truncated.starts_with(str_view));
}

/// @test `ends_with` returns `true` if and only if the length of the `string_view` is greater than or equal to the
/// length of the suffix, and the final characters of the `string_view` are equal to the suffix.
TEST(StringView, EndsWith) {
  constexpr string_view str_view{"hello"};
  constexpr string_view str_view2{"ello"};
  constexpr string_view str_view3{"goodbye"};

  STATIC_ASSERT_TRUE(str_view.ends_with(str_view));
  STATIC_ASSERT_TRUE(str_view.ends_with(str_view2));
  STATIC_ASSERT_FALSE(str_view.ends_with(str_view3));
  STATIC_ASSERT_FALSE(str_view.ends_with(str_view3.data()));
  STATIC_ASSERT_TRUE(str_view.ends_with(str_view2.data()));
  STATIC_ASSERT_FALSE(str_view.ends_with("world hello"));
  STATIC_ASSERT_FALSE(str_view.ends_with("hello world"));
  struct wrapper {
    string_view str_view;

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator char const*() const noexcept { return str_view.data(); }
  };

  STATIC_ASSERT_TRUE(str_view.ends_with(wrapper{str_view}));

  STATIC_ASSERT_TRUE(str_view.ends_with('o'));
  STATIC_ASSERT_FALSE(str_view.ends_with('x'));
  STATIC_ASSERT_FALSE(string_view{}.ends_with('x'));

  STATIC_ASSERT_TRUE(noexcept(str_view.ends_with('o')));
  STATIC_ASSERT_TRUE(noexcept(str_view.ends_with("o")));
  STATIC_ASSERT_TRUE(noexcept(str_view.ends_with(str_view)));

  string_view const str_view4{"Goodbye world"};
  ASSERT_FALSE(str_view4.ends_with(str_view3));
  ASSERT_TRUE(str_view4.ends_with("world"));
  ASSERT_FALSE(str_view4.ends_with('a'));
}

/// @test Invoking `find` with the same `string_view` returns 0
TEST(StringView, FindSelf) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.find(str_view), 0);
}

/// @test Invoking `find` with a string that is not a substring of the view returns `npos`
TEST(StringView, UnmatchingStringNotFound) {
  constexpr string_view str_view{"hello world"};
  constexpr string_view str_view2{"goodbye"};
  STATIC_ASSERT_EQ(str_view.find(str_view2), string_view::npos);
}

/// @test Invoking `find` with a string that is equal to a substring of the view returns the offset of the occurrence of
/// that substring
TEST(StringView, SubstringFound) {
  constexpr string_view str_view{"hello world"};
  constexpr string_view str_view2{"wor"};
  STATIC_ASSERT_EQ(str_view.find(str_view2), 6);
  STATIC_ASSERT_EQ(str_view.find(string_view{}), 0);
  STATIC_ASSERT_EQ(string_view{}.find(string_view{}), 0);
  STATIC_ASSERT_EQ(str_view.find("o"_asv), 4);
}

/// @test Invoking `find` with a string and a position returns the offset of the occurrence of that substring greater
/// than the specified position, or `npos` if there is no such position.
TEST(StringView, FindWithPosition) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.find("o"_asv, 5), 7);
  STATIC_ASSERT_EQ(str_view.find("hello"_asv, 1), string_view::npos);
  STATIC_ASSERT_EQ(str_view.find("hello"_asv, string_view::npos), string_view::npos);
}

/// @test Invoking `find` with a character returns the first position of that character in the string
TEST(StringView, FindSingleChar) {
  constexpr auto str_view = "test"_asv;
  STATIC_ASSERT_EQ(str_view.find('t'), 0);
  STATIC_ASSERT_EQ(str_view.find('s'), 2);
}

/// @test Invoking `find` with a single character and a position returns the first index of the character in the string
/// after the specified position
TEST(StringView, FindSingleCharWithStartPosition) {
  constexpr auto str_view = "test"_asv;
  STATIC_ASSERT_EQ(str_view.find('t', 1), 3);
  STATIC_ASSERT_EQ(str_view.find('e', 2), string_view::npos);
}

/// @test Invoking `find` with an empty string returns 0
TEST(StringView, FindEmptyString) {
  constexpr auto str_view = "test"_asv;
  STATIC_ASSERT_EQ(str_view.find(string_view{}), 0);
  STATIC_ASSERT_EQ(string_view{}.find(string_view{}), 0);
}

/// @test Invoking `find` with something implicitly convertible to a pointer to a NUL-terminated string returns the
/// first index of the characters in the converted string in the `string_view`
TEST(StringView, SubstringFoundWithImplicitConversion) {
  constexpr string_view str_view{"hello world"};
  constexpr string_view str_view2{"wor"};
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{str_view2}), 6);
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{{""}}), 0);
  STATIC_ASSERT_EQ(string_view{}.find(implicit_conversion_wrapper{{""}}), 0);
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{"o"_asv}), 4);
}

/// @test Invoking `find` with something implicitly convertible to a pointer to a NUL-terminated string and a position
/// returns the first index of the characters in the converted string in the `string_view` greater than the position, or
/// `npos` if there is no such index
TEST(StringView, FindWithPositionWithImplicitConversion) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{"o"_asv}, 5), 7);
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{"hello"_asv}, 1), string_view::npos);
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{"hello"_asv}, string_view::npos), string_view::npos);
}

/// @test Invoking `find` with a character and a position and count returns the first index from the specified position
/// of the string of the specified length from the character pointer
TEST(StringView, FindWithPositionAndCount) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.find(implicit_conversion_wrapper{str_view.substr(4)}, 5, 1), 7);
  constexpr string_view str_view2{"hello hell"};
  STATIC_ASSERT_EQ(str_view2.find(implicit_conversion_wrapper{str_view2}, 1, 4), 6);
  STATIC_ASSERT_EQ(str_view.find("hello", string_view::npos, 3), string_view::npos);
}

/// @test Invoking `rfind` with the whole string returns zero
TEST(StringView, ReverseFindSelf) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.rfind(str_view), 0);
}

/// @test Invoking `rfind` with a string that is not present returns `npos`
TEST(StringView, UnmatchingStringNotFoundInReverse) {
  constexpr string_view str_view{"hello world"};
  constexpr string_view str_view2{"goodbye"};
  STATIC_ASSERT_EQ(str_view.rfind(str_view2), string_view::npos);
}

/// @test Invoking `rfind` with a substring returns the index of that substring
TEST(StringView, SubstringFoundInReverse) {
  constexpr string_view str_view{"hello world"};
  constexpr string_view str_view2{"wor"};
  STATIC_ASSERT_EQ(str_view.rfind(str_view2), 6);
  STATIC_ASSERT_EQ(str_view.rfind(string_view{}), str_view.length());
  STATIC_ASSERT_EQ(string_view{}.rfind(string_view{}), 0);
  STATIC_ASSERT_EQ(str_view.rfind("o"_asv), 7);
}

/// @test Invoking `rfind` with a substring and a position returns the index of that substring that is less than the
/// specified position
TEST(StringView, ReverseFindWithPosition) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.rfind("o"_asv, 5), 4);
  STATIC_ASSERT_EQ(str_view.rfind("world"_asv, 5), string_view::npos);
  STATIC_ASSERT_EQ(str_view.rfind("hello"_asv, string_view::npos), 0);
}

/// @test Invoking `rfind` with a single character returns the index of the last instance of that character
TEST(StringView, ReverseFindSingleChar) {
  STATIC_ASSERT_EQ(string_view{"test"}.rfind('x'), string_view::npos);
  STATIC_ASSERT_EQ(string_view{"test"}.rfind('t'), 3);
  STATIC_ASSERT_EQ(string_view{"test"}.rfind('s'), 2);
}

/// @test Invoking `rfind` with an empty string returns the length of the `string_view`
TEST(StringView, ReverseFindEmptyString) {
  constexpr auto str_view = "test"_asv;
  STATIC_ASSERT_EQ(str_view.rfind(string_view{}), str_view.size());
  STATIC_ASSERT_EQ(string_view{}.rfind(string_view{}), 0);
}

/// @test Invoking `rfind` with a single character and a position returns the index of the last instance of that
/// character less than the specified position
TEST(StringView, ReverseFindSingleCharWithStartPosition) {
  STATIC_ASSERT_EQ(string_view{"test"}.rfind('x', 0), string_view::npos);
  STATIC_ASSERT_EQ(string_view{"test"}.rfind('t', 1), 0);
  STATIC_ASSERT_EQ(string_view{"test"}.rfind('s', 1), string_view::npos);
}

/// @test Invoking `rfind` with something implicitly convertible to a pointer to a NUL-terminated string returns the
/// index of the last instance of the pointed-to string
TEST(StringView, SubstringFoundWithImplicitConversionInReverse) {
  constexpr string_view str_view{"hello world"};
  constexpr string_view str_view2{"wor"};
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{str_view2}), 6);
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{{""}}), str_view.length());
  STATIC_ASSERT_EQ(string_view{}.rfind(implicit_conversion_wrapper{{""}}), 0);
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{"o"_asv}), 7);
}

/// @test Invoking `rfind` with something implicitly convertible to a pointer to a NUL-terminated string and a position
/// returns the index of the last instance of the pointed-to string less than the specified position
TEST(StringView, ReverseFindWithPositionWithImplicitConversion) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{"o"_asv}, 5), 4);
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{"world"_asv}, 1), string_view::npos);
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{"hello"_asv}, string_view::npos), 0);
}

/// @test Invoking `rfind` with a character pointer and a position and length returns the index of the last
/// instance of the string consisting of the specified number of characters from the pointer that is less than the
/// position
TEST(StringView, ReverseFindWithPositionAndCount) {
  constexpr string_view str_view{"hello world"};
  STATIC_ASSERT_EQ(str_view.rfind(implicit_conversion_wrapper{str_view.substr(4)}, 5, 1), 4);
  constexpr string_view str_view2{"hell hello"};
  STATIC_ASSERT_EQ(str_view2.rfind(implicit_conversion_wrapper{str_view2}, 7, 4), 5);
  STATIC_ASSERT_EQ(str_view.rfind("good", string_view::npos, 3), string_view::npos);
}

/// @test Invoking `find_first_of` with a list of characters that aren't present returns `npos`
TEST(StringView, FindFirstOfNonMatchingList) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars = "abc";

  STATIC_ASSERT_EQ(str_view.find_first_of(""), string_view::npos);
  STATIC_ASSERT_EQ(str_view.find_first_of(chars), string_view::npos);
}

/// @test Invoking `find_first_of` with a list of characters where at least one is present returns the index of the
/// first character in the string that is in the list
TEST(StringView, FindFirstOfMatchingList) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";
  constexpr string_view chars2 = "hw";

  STATIC_ASSERT_EQ(str_view.find_first_of(chars1), 1);
  STATIC_ASSERT_EQ(str_view.find_first_of(chars2), 0);
}

/// @test Invoking `find_first_of` with a list of characters and an offset where at least one is present returns the
/// index of the first character in the string that is in the list after the specified offset
TEST(StringView, FindFirstOfWithOffset) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_of(""_asv, 2), string_view::npos);
  STATIC_ASSERT_EQ(str_view.find_first_of("abcde"_asv, 2), 10);
  STATIC_ASSERT_EQ(str_view.find_first_of("hw"_asv, 1), 6);
  STATIC_ASSERT_EQ(str_view.find_first_of("fg%"_asv, 2), string_view::npos);
}

/// @test Invoking `find_first_of` with a list of characters specified by a NUL-terminated string where at least one is
/// present returns the index of the first character in the string that is in the list
TEST(StringView, FindFirstOfMatchingListWithImplicitConversion) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";
  constexpr string_view chars2 = "hw";

  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{chars1}), 1);
  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{chars2}), 0);
}

/// @test Invoking `find_first_of` with a list of characters specified by a NUL-terminated string and an offset where at
/// least one is present returns the index of the first character in the string that is in the list after the specified
/// offset
TEST(StringView, FindFirstOfWithOffsetWithImplicitConversion) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{"abcde"_asv}, 2), 10);
  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{"hw"_asv}, 1), 6);
  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{"fg%"_asv}, 2), string_view::npos);
}

/// @test Invoking `find_first_of` with a single character returns the index of that character, or `npos` if it is not
/// found
TEST(StringView, FindFirstOfWithCharacter) {
  constexpr string_view str_view = "hello world!";

  STATIC_ASSERT_EQ(str_view.find_first_of('e'), 1);
  STATIC_ASSERT_EQ(str_view.find_first_of('h'), 0);
  STATIC_ASSERT_EQ(str_view.find_first_of('q'), string_view::npos);
}

/// @test Invoking `find_first_of` with a single character and an offset returns the index of that character after the
/// offset, or `npos` if it is not found
TEST(StringView, FindFirstOfCharacterWithOffset) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_of('l', 5), 9);
  STATIC_ASSERT_EQ(str_view.find_first_of('q', 1), string_view::npos);
}

/// @test Invoking `find_first_of` with a list of characters specified by a string literal and an offset and length
/// where at least one is present returns the index of the first character in the string that is in the list specified
/// as the string of the specified length from the character pointer, after the specified offset in the searched string
TEST(StringView, FindFirstOfMatchingListWithOffsetAndLength) {
  constexpr string_view str_view = "hello world!";

  STATIC_ASSERT_EQ(str_view.find_first_of("abcdef", 0, 4), 10);
  STATIC_ASSERT_EQ(str_view.find_first_of("helow", 2, 2), string_view::npos);
}

/// @test Invoking `find_first_of` with a list of characters specified by a character pointer and an offset and length
/// where at least one is present returns the index of the first character in the string that is in the list specified
/// as the string of the specified length from the character pointer, after the specified offset in the searched string
TEST(StringView, FindFirstOfMatchingListWithImplicitConversionAndLength) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";
  constexpr string_view chars2 = "hw";

  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{chars1}, 0, 4), 10);
  STATIC_ASSERT_EQ(str_view.find_first_of(implicit_conversion_wrapper{chars2}, 2, 1), string_view::npos);
}

/// @test Invoking `find_first_not_of` with a list of characters returns the index of the first character not in the
/// list, or `npos` if all the characters in the string are in the list.
TEST(StringView, FindFirstNotOf) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_not_of(""), 0);
  STATIC_ASSERT_EQ(str_view.find_first_not_of("abc"_asv), 0);
  STATIC_ASSERT_EQ(str_view.find_first_not_of(str_view), string_view::npos);
}

/// @test Invoking `find_first_not_of` with a list of characters and an offset returns the index of the first character
/// not in the list after the offset, or `npos` if all the characters in the string after the offset are in the list.
TEST(StringView, FindFirstNotOfWithOffset) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_not_of("", 1), 1);
  ASSERT_EQ(str_view.find_first_not_of("abcde"_asv, 2), 2);
  ASSERT_EQ(str_view.find_first_not_of(" elo"_asv, 1), 6);
  ASSERT_EQ(str_view.find_first_not_of("world! "_asv, 2), string_view::npos);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a NUL-terminated string returns the index
/// of the first character not in the list, or `npos` if all the characters in the string are in the list.
TEST(StringView, FindFirstNotOfMatchingListWithImplicitConversion) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";

  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{chars1}), 0);
  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{str_view}), string_view::npos);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a NUL-terminated string and an offset
/// returns the index of the first character not in the list after the offset, or `npos` if all the characters in the
/// string after the offset are in the list.
TEST(StringView, FindFirstNotOfWithOffsetWithImplicitConversion) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{"abcde"_asv}, 2), 2);
  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{"elo "_asv}, 1), 6);
  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{"world! %^zz"_asv}, 2), string_view::npos);
}

/// @test Invoking `find_first_not_of` with a single character returns the index of the first character
/// not equal to that character, or `npos` if all the characters in the string are the same as the character
TEST(StringView, FindFirstNotOfWithCharacter) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_not_of('e'), 0);
  STATIC_ASSERT_EQ(str_view.find_first_not_of('h'), 1);
  STATIC_ASSERT_EQ("a"_asv.find_first_not_of('a'), string_view::npos);
}

/// @test Invoking `find_first_not_of` with a single character and an offset returns the index of the first character
/// not equal to the character after the offset
TEST(StringView, FindFirstNotOfCharacterWithOffset) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_first_not_of('h', 5), 5);
  STATIC_ASSERT_EQ(str_view.find_first_not_of('l', 2), 4);
}

/// @test Invoking `find_first_not_of` with a list of characters specified by a character pointer and an offset and
/// length returns the index of the first character not in the string of characters of the specified length from the
/// pointer after the offset, or `npos` if all the characters in the string after the offset are in the list.
TEST(StringView, FindFirstNotOfMatchingListWithImplicitConversionAndLength) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";
  constexpr string_view chars2 = "eorld! w";

  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{chars1}, 1, 4), 1);
  STATIC_ASSERT_EQ(str_view.find_first_not_of(implicit_conversion_wrapper{chars2}, 1, 1), 2);
}

/// @test Invoking `find_last_of` with a list of characters returns the index of the last character in the string that
/// is in the list, or `npos` if none of the characters are in the list
TEST(StringView, FindLastOf) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of(""), string_view::npos);
  STATIC_ASSERT_EQ(str_view.find_last_of("abcd"_asv), 15);
  STATIC_ASSERT_EQ(str_view.find_last_of(str_view), str_view.length() - 1);
  STATIC_ASSERT_EQ(str_view.find_last_of("q"), string_view::npos);
}

/// @test Invoking `find_last_of` with a list of characters and an offset returns the index of the last character in the
/// string before the offset that is in the list, or `npos` if none of the characters before the offset are in the
/// list
TEST(StringView, FindLastOfWithOffset) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of("", 1), string_view::npos);
  STATIC_ASSERT_EQ(str_view.find_last_of("abcd"_asv, 8), 4);
  STATIC_ASSERT_EQ(str_view.find_last_of("abcd"_asv, 3), 3);
  STATIC_ASSERT_EQ(str_view.find_last_of("abcd"_asv, 2), string_view::npos);
}

/// @test Invoking `find_last_of` with a list of characters specified by a NUL-terminated string returns the index of
/// the last character in the string that is in the list, or `npos` if none of the characters are in the list
TEST(StringView, FindLastOfWithCharPtrConversion) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of(implicit_conversion_wrapper{"abcd"_asv}), 15);
  STATIC_ASSERT_EQ(str_view.find_last_of(str_view), str_view.length() - 1);
}

/// @test Invoking `find_last_of` with a list of characters specified by a NUL-terminated string and an offset returns
/// the index of the last character in the string before the offset that is in the list, or `npos` if none of the
/// characters before the offset are in the list
TEST(StringView, FindLastOfWithOffsetWithCharPtrConversion) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of(implicit_conversion_wrapper{"abcd"_asv}, 8), 4);
  STATIC_ASSERT_EQ(str_view.find_last_of(implicit_conversion_wrapper{"abcd"_asv}, 3), 3);
  STATIC_ASSERT_EQ(str_view.find_last_of(implicit_conversion_wrapper{"abcd"_asv}, 2), string_view::npos);
}

/// @test Invoking `find_last_of` with a list of characters specified by a character pointer and an offset and a count
/// returns the index of the last character in the string before the offset that is in a list made of the first N
/// characters from the character pointer, or `npos` if none of the characters before the offset are in the list
TEST(StringView, FindLastOfWithOffsetAndCountWithCharPtrConversion) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of(implicit_conversion_wrapper{"abcde"_asv}, 8, 4), 4);
  STATIC_ASSERT_EQ(str_view.find_last_of("gxye", 8, 2), 0);
  STATIC_ASSERT_EQ(str_view.find_last_of(implicit_conversion_wrapper{"abcdefg"_asv}, 2, 4), string_view::npos);
}

/// @test Invoking `find_last_of` with a single character returns the last index of that character, or `npos` if it is
/// not found
TEST(StringView, FindLastOfChar) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of('d'), 15);
  STATIC_ASSERT_EQ(str_view.find_last_of('x'), string_view::npos);
}

/// @test Invoking `find_last_of` with a single character and an offset returns the index of that character before the
/// offset, or `npos` if it is not found
TEST(StringView, FindLastOfCharWithOffset) {
  constexpr string_view str_view = "goodbye everybody!";
  STATIC_ASSERT_EQ(str_view.find_last_of('d', 8), 3);
  STATIC_ASSERT_EQ(str_view.find_last_of('y', 2), string_view::npos);
}

/// @test Invoking `find_last_not_of` with a list of characters returns the index of the last character not in the
/// list, or `npos` if all the characters in the string are in the list.
TEST(StringView, FindLastNotOf) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_last_not_of(""_asv), str_view.length() - 1);
  STATIC_ASSERT_EQ(str_view.find_last_not_of("abc"_asv), str_view.length() - 1);
  STATIC_ASSERT_EQ(str_view.find_last_not_of(str_view), string_view::npos);
}

/// @test Invoking `find_last_not_of` with a list of characters and an offset returns the index of the last character
/// not in the list before the offset, or `npos` if all the characters in the string before the offset are in the list.
TEST(StringView, FindLastNotOfWithOffset) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_last_not_of(""_asv, 1), 1);
  STATIC_ASSERT_EQ(str_view.find_last_not_of(""_asv, str_view.size()), str_view.size() - 1U);
  STATIC_ASSERT_EQ(str_view.find_last_not_of("abcde"_asv, 10), 9);
  STATIC_ASSERT_EQ(str_view.find_last_not_of("elowr"_asv, 9), 5);
  STATIC_ASSERT_EQ(str_view.find_last_not_of("whelo "_asv, 7), string_view::npos);
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a NUL-terminated string returns the index
/// of the last character not in the list, or `npos` if all the characters in the string are in the list.
TEST(StringView, FindLastNotOfMatchingListWithImplicitConversion) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";

  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{chars1}), str_view.length() - 1);
  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{str_view}), string_view::npos);
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a NUL-terminated string and an offset
/// returns the index of the last character not in the list before the offset, or `npos` if all the characters in the
/// string before the offset are in the list.
TEST(StringView, FindLastNotOfWithOffsetWithImplicitConversion) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{"abcde"_asv}, 10), 9);
  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{"elowr"_asv}, 9), 5);
  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{"whelo "_asv}, 7), string_view::npos);
}

/// @test Invoking `find_last_not_of` with a single character returns the index of the last character
/// not equal to that character, or `npos` if all the characters in the string are the same as the character
TEST(StringView, FindLastNotOfWithCharacter) {
  constexpr string_view str_view = "hello world!";

  STATIC_ASSERT_EQ(str_view.find_last_not_of('e'), str_view.length() - 1);
  STATIC_ASSERT_EQ(str_view.find_last_not_of('!'), str_view.length() - 2);
  STATIC_ASSERT_EQ("a"_asv.find_last_not_of('a'), string_view::npos);
}

/// @test Invoking `find_last_not_of` with a single character and an offset returns the index of the last character
/// not equal to the character before the offset
TEST(StringView, FindLastNotOfCharacterWithOffset) {
  constexpr string_view str_view = "hello world!";
  STATIC_ASSERT_EQ(str_view.find_last_not_of('h', 5), 5);
  STATIC_ASSERT_EQ(str_view.find_last_not_of('l', 3), 1);
}

/// @test Invoking `find_last_not_of` with a list of characters specified by a character pointer and an offset and
/// length returns the index of the last character not in the string of characters of the specified length from the
/// pointer before the offset, or `npos` if all the characters in the string before the offset are in the list.
TEST(StringView, FindLastNotOfMatchingListWithImplicitConversionAndLength) {
  constexpr string_view str_view = "hello world!";
  constexpr string_view chars1 = "abcdef";
  constexpr string_view chars2 = "eorld! w";

  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{chars1}, 1, 4), 1);
  STATIC_ASSERT_EQ(str_view.find_last_not_of(implicit_conversion_wrapper{chars2}, 1, 1), 0);
}

namespace {
struct copy_result {
  array<char, 20> data{};
  string_view::size_type count = 0;
};

constexpr auto do_copy(string_view str_view, string_view::size_type n, string_view::size_type pos) -> copy_result {
  copy_result res;
  res.count = str_view.copy(res.data.data(), n, pos);
  return res;
}

constexpr auto do_copy(string_view str_view, string_view::size_type n) -> copy_result {
  copy_result res;
  res.count = str_view.copy(res.data.data(), n);
  return res;
}

}  // namespace

/// @test Invoking `copy` copies the specified number of characters into the specified buffer
TEST(StringView, Copy) {
  constexpr string_view str_view = "hello world";

  constexpr std::size_t count = 5;
  STATIC_ASSERT_EQ(do_copy(str_view, count).count, count);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[0], 'h');
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[1], 'e');
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[2], 'l');
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[3], 'l');
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[4], 'o');
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[5], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[6], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[7], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[8], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[9], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[10], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[11], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[12], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[13], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[14], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[15], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[16], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[17], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[18], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count).data[19], 0);

  auto const str_view2 = str_view;
  auto const copy = do_copy(str_view2, count);

  ASSERT_EQ(string_view(copy.data.data(), copy.count), str_view.substr(0, count));
}

/// @test Invoking `copy` with a count and position copies the specified number of characters into the specified buffer,
/// starting with the specified offset in the string.
TEST(StringView, CopyWithPos) {
  constexpr string_view str_view = "hello world";

  constexpr std::size_t count = 10;
  constexpr std::size_t pos = 5;
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).count, 6);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[0], ' ');
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[1], 'w');
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[2], 'o');
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[3], 'r');
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[4], 'l');
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[5], 'd');
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[6], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[7], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[8], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[9], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[10], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[11], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[12], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[13], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[14], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[15], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[16], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[17], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[18], 0);
  STATIC_ASSERT_EQ(do_copy(str_view, count, pos).data[19], 0);
}

/// @test Invoking `copy` with a position that is larger than the string does nothing
TEST(StringView, CopyCopiesNothingIfPosIsLargerThanString) {
  constexpr string_view str_view = "hello world";

  STATIC_ASSERT_EQ(do_copy(str_view, str_view.size(), str_view.size()).count, 0);
  STATIC_ASSERT_EQ(do_copy(str_view, str_view.size(), str_view.size() + 10).count, 0);
}

/// @test `begin` and `end` are `noexcept`
TEST(StringView, BeginAndEndAreNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<string_view&>().begin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<string_view&>().end()));

  STATIC_ASSERT_TRUE(noexcept(std::declval<string_view&>().cbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<string_view&>().cend()));
}

/// @test `begin` and `end` and `cbegin` and `cend` return `const_iterator`s which can be used to iterate through the
/// elements of the view
TEST(StringView, ConstIterators) {
  constexpr string_view str_view = "hello everyone";

  STATIC_ASSERT_EQ(str_view.begin(), str_view.cbegin());
  STATIC_ASSERT_EQ(str_view.end(), str_view.cend());
  ::testing::StaticAssertTypeEq<decltype(str_view.begin()), decltype(str_view.cbegin())>();
  ::testing::StaticAssertTypeEq<decltype(str_view.end()), decltype(str_view.cend())>();

  string_view const str_view2 = str_view;
  auto itr = str_view.begin();
  for (auto citr = str_view2.cbegin(); citr != str_view2.cend(); consume_values({++citr, ++itr})) {
    ASSERT_EQ(itr, citr);
    ASSERT_EQ(*itr, *citr);
  }
}

/// @test `rbegin` and `rend` return `reverse_iterator`s which can be used to iterate through the view in reverse.
TEST(StringView, ReverseIterators) {
  constexpr string_view str_view = "hello everyone";

  ::testing::StaticAssertTypeEq<decltype(str_view.rbegin()), string_view::reverse_iterator>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<string_view::reverse_iterator>::reference, string_view::reference>(
  );
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<string_view::reverse_iterator>);

  ASSERT_EQ(&*str_view.rbegin(), &str_view.back());

  ::testing::StaticAssertTypeEq<decltype(str_view.rend()), string_view::reverse_iterator>();

  ASSERT_EQ(&*(str_view.rend() - 1), &str_view.front());

  array<char, max_string_length> arr{};
  span<char, dynamic_extent> const view{arr.data(), str_view.length()};

  auto view_itr = view.begin();
  for (string_view::reverse_iterator itr = str_view.rbegin(); itr != str_view.rend(); ++itr, ++view_itr) {
    *view_itr = *itr;
  }

  ASSERT_TRUE(arene::base::equal(view.rbegin(), view.rend(), str_view.begin(), str_view.end()));
}

/// @test `crbegin` and `crend` return `reverse_iterator`s which can be used to iterate through the view in reverse.
TEST(StringView, ConstReverseIterators) {
  constexpr string_view str_view = "hello everyone";

  ::testing::StaticAssertTypeEq<decltype(str_view.crbegin()), string_view::const_reverse_iterator>();
  ::testing::
      StaticAssertTypeEq<std::iterator_traits<string_view::const_reverse_iterator>::reference, string_view::reference>(
      );
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<string_view::const_reverse_iterator>);

  ASSERT_EQ(&*str_view.crbegin(), &str_view.back());

  ::testing::StaticAssertTypeEq<decltype(str_view.crend()), string_view::const_reverse_iterator>();

  ASSERT_EQ(&*(str_view.crend() - 1), &str_view.front());

  array<char, max_string_length> arr{};
  span<char, dynamic_extent> const view{arr.data(), str_view.length()};

  auto view_itr = view.begin();
  for (string_view::const_reverse_iterator itr = str_view.crbegin(); itr != str_view.crend(); ++itr, ++view_itr) {
    *view_itr = *itr;
  }

  ASSERT_TRUE(arene::base::equal(view.rbegin(), view.rend(), str_view.begin(), str_view.end()));
}

/// @test Invoking `compare` returns a result which is consitent with lexicographical ordering with respect to the
/// compared string
TEST(StringViewCompare, ReturnsLexicographicalComparisonWithOther) {
  constexpr string_view str_view1 = "hello world";
  constexpr string_view str_view2 = "hello world!";
  constexpr string_view str_view3 = "hello";
  constexpr string_view str_view4 = "hello again";
  constexpr string_view str_view5 = "offset hello world";
  constexpr string_view str_view6 = "embedded hello world with suffix";
  constexpr string_view str_view7 = "world";

  STATIC_ASSERT_EQ(str_view1.compare(str_view1), 0);
  STATIC_ASSERT_TRUE(noexcept(str_view1.compare(str_view1)));
  STATIC_ASSERT_LT(str_view1.compare(str_view2), 0);
  STATIC_ASSERT_GT(str_view1.compare(str_view3), 0);
  STATIC_ASSERT_GT(str_view1.compare(str_view4), 0);
  STATIC_ASSERT_EQ(str_view1.compare(0, 5, str_view3), 0);
  STATIC_ASSERT_LT(str_view1.compare(0, 5, str_view7), 0);
  STATIC_ASSERT_EQ(str_view1.compare(6, 5, str_view7), 0);
  STATIC_ASSERT_GT(str_view1.compare(6, 5, str_view3), 0);

  STATIC_ASSERT_EQ(str_view1.compare(0, 5, str_view5, 7, 5), 0);
  STATIC_ASSERT_EQ(str_view1.compare(0, string_view::npos, str_view5, 7, 11), 0);

#if !ARENE_IS_ON(ARENE_COMPILER_GCC) || __GNUC__ > 8
  STATIC_ASSERT_TRUE(noexcept(str_view1.compare(6, 5, str_view3)));
  STATIC_ASSERT_TRUE(noexcept(str_view1.compare(0, 5, str_view5, 7, 5)));
#endif

  STATIC_ASSERT_EQ(str_view1.compare(0, string_view::npos, str_view5, 7, string_view::npos), 0);
  STATIC_ASSERT_EQ(str_view1.compare(3, string_view::npos, str_view6, 12, 8), 0);
  STATIC_ASSERT_LT(str_view1.compare(3, string_view::npos, str_view6, 12, 9), 0);
  STATIC_ASSERT_GT(str_view1.compare(3, string_view::npos, str_view6, 12, 7), 0);
}

/// @test Invoking `compare` with a position greater than size is less than a non-empty string
TEST(StringViewCompare, WithStringViewReturnsLessIfPos1GreaterThanSize) {
  constexpr string_view str = "hello";
  constexpr string_view other = "doesn't matter";
  STATIC_ASSERT_LT(str.compare(str.size() + 1, str.size(), other), 0);
  STATIC_ASSERT_LT(str.compare(str.size() + 1, str.size(), other, 0, other.size()), 0);
}

/// @test Invoking `compare` with a position into the "other" string greater than size means the first string is greater
TEST(StringViewCompare, WithStringViewReturnsGreaterIfPos2GreaterThanOtherSize) {
  constexpr string_view str = "hello";
  constexpr string_view other = "doesn't matter";
  STATIC_ASSERT_GT(str.compare(0, str.size(), other, other.size() + 1, other.size()), 0);
}

/// @test Invoking `compare` with NUL-terminated strings returns results consistent with lexicographical ordering
TEST(StringView, ComparisonsWithRawStrings) {
  constexpr string_view str_view1{"hello world"};
  constexpr implicit_conversion_wrapper icw2{{"hello world!"}};
  constexpr implicit_conversion_wrapper icw3{{"hello"}};
  constexpr implicit_conversion_wrapper icw4{{"hello again"}};
  constexpr implicit_conversion_wrapper icw5{{"words"}};
  constexpr implicit_conversion_wrapper icw6{{"world"}};

  STATIC_ASSERT_LT(str_view1.compare(icw2), 0);
  STATIC_ASSERT_GT(str_view1.compare(icw3), 0);
  STATIC_ASSERT_GT(str_view1.compare(icw4), 0);
  STATIC_ASSERT_EQ(str_view1.compare(0, 5, icw3), 0);
  STATIC_ASSERT_LT(str_view1.compare(0, 5, icw6), 0);
  STATIC_ASSERT_EQ(str_view1.compare(6, 5, icw6), 0);
  STATIC_ASSERT_GT(str_view1.compare(6, 5, icw3), 0);
  STATIC_ASSERT_EQ(str_view1.compare(0, 5, icw4, 5), 0);

  STATIC_ASSERT_EQ(str_view1.compare(6, 3, icw5, 3), 0);
  STATIC_ASSERT_LT(str_view1.compare(6, 3, icw5, 4), 0);
  STATIC_ASSERT_GT(str_view1.compare(6, 4, icw5, 4), 0);
}

/// @test `swap` exchanges the `data` and `size` of `string_view` objects
TEST(StringView, Swap) {
  constexpr string_view hello{"hello"};
  constexpr string_view goodbye{"goodbye"};

  auto str_view1 = hello;
  auto str_view2 = goodbye;

  STATIC_ASSERT_TRUE(noexcept(str_view1.swap(str_view2)));
  str_view1.swap(str_view2);
  ASSERT_EQ(str_view1, goodbye);
  ASSERT_EQ(str_view2, hello);
  ASSERT_EQ(str_view1.data(), goodbye.data());
  ASSERT_EQ(str_view2.data(), hello.data());
}

/// @test The `max_size` of a `string_view` is the max value of `std::size_t`
TEST(StringView, MaxSize) {
  STATIC_ASSERT_EQ(string_view::max_size(), std::numeric_limits<std::size_t>::max());
  STATIC_ASSERT_LT(0, string_view::max_size());
}

/// @test Constructing a `string_view` with a source string and a length gives a view over the whole length, even if the
/// source string has embedded NUL characters
TEST(StringView, StringViewNullByteInInput) {
  constexpr std::size_t msg_len = 5;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static constexpr char msg_nullbyte_constchar[msg_len] = {'\xDE', '\x00', '\xAD', '\xBE', '\xEF'};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  constexpr string_view full(msg_nullbyte_constchar, msg_len);

  constexpr std::size_t computed_len = string_length(full.data());

  STATIC_ASSERT_NE(computed_len, full.size());

  STATIC_ASSERT_EQ(full.size(), msg_len);
  STATIC_ASSERT_FALSE(full.empty());
  STATIC_ASSERT_EQ(full.size(), full.length());
  STATIC_ASSERT_NE(full.length(), computed_len);
  STATIC_ASSERT_NE(full.length(), string_length(full.data()));

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  ASSERT_NE(full.size(), std::string(&msg_nullbyte_constchar[0]).size());
#endif
}

}  // namespace
