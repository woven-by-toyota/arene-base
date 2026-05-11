// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/cstring.hpp"

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

auto empty_tests() -> void {
  using ::testing::cstring::empty;

  static_assert(empty(""), "empty string is empty");

  static_assert(!empty("a"), "single element string is not empty");
  static_assert(!empty("aa"), "two element string is not empty");
  static_assert(!empty("ab"), "two element string is not empty");
  static_assert(!empty("abc"), "three element string is not empty");
}

auto size_tests() -> void {
  using ::testing::cstring::size;

  static_assert(size("") == 0, "empty string has length 0");

  static_assert(size("a") == 1, "single element string has length 1");

  static_assert(size("aa") == 2, "two element string has length 2");
  static_assert(size("ab") == 2, "two element string has length 2");

  static_assert(size("abc") == 3, "three element string has length 3");
}

auto starts_with_tests() -> void {
  using ::testing::cstring::starts_with;

  static_assert(starts_with("", ""), "an empty string starts with an empty string");
  static_assert(starts_with("a", ""), "a non-empty string starts with an empty string");
  static_assert(starts_with("ab", ""), "a non-empty string starts with an empty string");

  static_assert(!starts_with("", "a"), "an empty string does not start with 'a'");
  static_assert(!starts_with("b", "a"), "'b' does not start with 'a'");
  static_assert(!starts_with("ba", "a"), "'ba' does not start with 'a'");

  static_assert(starts_with("a", "a"), "'a' starts with 'a'");
  static_assert(starts_with("aa", "a"), "'aa' starts with 'a'");
  static_assert(starts_with("ab", "a"), "'ab' starts with 'a'");

  static_assert(starts_with("abc", "abc"), "'abc' starts with 'abc'");
  static_assert(starts_with("abca", "abc"), "'abca' starts with 'abc'");
  static_assert(!starts_with("aabc", "abc"), "'aabc' does not start with 'abc'");
}

auto contains_tests() -> void {
  using ::testing::cstring::contains;

  {
    static constexpr auto str = "";

    static_assert(contains(str, ""), "the empty substring is always contained");
    static_assert(!contains(str, "a"), "'a' is not contained in an empty string");
    static_assert(!contains(str, "ab"), "'ab' is not contained in an empty string");
  }

  {
    static constexpr auto str = "abc";

    static_assert(contains(str, ""), "the empty substring is always contained");
    static_assert(contains(str, "a"), "'a' starts at 'str'");
    static_assert(contains(str, "ab"), "'ab' starts at 'str'");
    static_assert(contains(str, "abc"), "'abc' starts at 'str'");

    static_assert(contains(str, "b"), "'b' starts at 'str + 1'");
    static_assert(contains(str, "c"), "'c' starts at 'str + 2'");
    static_assert(contains(str, "bc"), "'bc' starts at 'str + 1'");

    static_assert(!contains(str, "d"), "'d' is not contained in 'str'");
    static_assert(!contains(str, "cb"), "'cb' is not contained in 'str'");
    static_assert(!contains(str, "ba"), "'ba' is not contained in 'str'");
  }
}

auto find_tests() -> void {
  using ::testing::cstring::find;

  {
    static constexpr auto str = "";

    static_assert(find(str, "") == str, "the empty substring starts at 'str'");
    static_assert(find(str, "a") == nullptr, "'a' is not contained in an empty string");
    static_assert(find(str, "ab") == nullptr, "'ab' is not contained in an empty string");
  }

  {
    static constexpr auto str = "abc";

    static_assert(find(str, "") == str, "the empty substring starts at 'str'");
    static_assert(find(str, "a") == str, "'a' starts at 'str'");
    static_assert(find(str, "ab") == str, "'ab' starts at 'str'");
    static_assert(find(str, "abc") == str, "'abc' starts at 'str'");

    static_assert(find(str, "b") == str + 1, "'b' starts at 'str + 1'");
    static_assert(find(str, "c") == str + 2, "'c' starts at 'str + 2'");
    static_assert(find(str, "bc") == str + 1, "'bc' starts at 'str + 1'");

    static_assert(find(str, "d") == nullptr, "'d' is not contained in 'str'");
    static_assert(find(str, "cb") == nullptr, "'cb' is not contained in 'str'");
    static_assert(find(str, "ba") == nullptr, "'ba' is not contained in 'str'");
  }
}

auto equal_tests() -> void {
  using ::testing::cstring::equal;

  static_assert(equal("", ""), "two empty strings are equal");
  static_assert(!equal("", "a"), "an empty string and a non-empty string are not equal");
  static_assert(!equal("a", ""), "an empty string and a non-empty string are not equal");

  static_assert(equal("abc", "abc"), "'abc' is equal to 'abc'");

  static_assert(!equal("abcd", "abc"), "'abcd' is not equal to 'abc'");
  static_assert(!equal("a", "b"), "'a' is not equal to 'b'");
}

auto main() -> int {}

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
