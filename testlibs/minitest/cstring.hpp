// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_CSTRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_CSTRING_HPP_

// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stddef.h>

// constexpr functions working with null-terminated byte strings

namespace testing {
namespace cstring {

/// @brief determine if a string is empty
/// @param str string
/// @return @c true if @c str is empty, otherwise @c false
/// @pre @c str is a valid null-terminated byte string
///
constexpr auto empty(char const* str) noexcept -> bool { return *str == '\0'; }

/// @brief get the size of a string
/// @param str string
/// @return number of characters in @c str
/// @pre @c str is a valid null-terminated byte string
///
constexpr auto size(char const* str) noexcept -> ::size_t {
  auto len = ::size_t{};

  while (*str != '\0') {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ++str;
    ++len;
  }

  return len;
}

/// @brief determine if @c str starts with @c prefix
/// @param str string which may contain a prefix
/// @param prefix the prefix to check for
/// @return @c true if @c str starts with @c prefix
/// @pre @c str is a valid null-terminated byte string
/// @pre @c prefix is a valid null-terminated byte string
///
constexpr auto starts_with(char const* str, char const* prefix) noexcept -> bool {
  while (*prefix != '\0') {
    if (*str != *prefix) {
      return false;
    }
    ++str;     // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ++prefix;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }

  return true;
}

/// @brief find a substring within a string
/// @param str string to search
/// @param sub substring to search for
/// @return iterator to the beginning of @c sub in @c str or nullptr if @c sub is not contained in @c str
/// @pre @c str is a valid null-terminated byte string
/// @pre @c sub is a valid null-terminated byte string
///
constexpr auto find(char const* str, char const* sub) noexcept -> char const* {
  if (*sub == '\0') {
    return str;
  }

  while (*str != '\0') {
    if (starts_with(str, sub)) {
      return str;
    }
    ++str;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }

  return nullptr;
}

/// @brief check if a substring is contained within a string
/// @param str string to search
/// @param sub substring to search for
/// @return @c true if @c sub is contained in <c>str</c>, @c false if not
/// @pre @c str is a valid null-terminated byte string
/// @pre @c sub is a valid null-terminated byte string
///
constexpr auto contains(char const* str, char const* sub) noexcept -> bool { return find(str, sub) != nullptr; }

/// @brief determine if two strings are equal
/// @param str1 first string to compare
/// @param str2 second string to compare
/// @return @c true if the contents of @c str1 and @c str2 are equal
/// @pre @c str1 is a valid null-terminated byte string
/// @pre @c str2 is a valid null-terminated byte string
///
constexpr auto equal(char const* str1, char const* str2) -> bool {
  while (*str1 == *str2 && *str1 != '\0') {
    ++str1;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ++str2;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }

  return *str1 == *str2;
}

}  // namespace cstring
}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_CSTRING_HPP_
