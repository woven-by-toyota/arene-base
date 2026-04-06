// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTS_TESTING_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTS_TESTING_TYPES_HPP_

// Prevent IWYU from including this header by redirecting it to a standard header
// IWYU pragma: private, include <cstddef>
// IWYU pragma: friend "arene/base/inline_container/tests/.*"

#include "arene/base/compiler_support/diagnostics.hpp"  // IWYU pragma: keep
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <istream>
#endif

namespace arene {
namespace base {
namespace testing {

// This class is for testing instantiation using a non-comparable user-defined literal type.
struct wrapped_char {
  char val;

  friend constexpr auto operator==(wrapped_char lhs, wrapped_char rhs) noexcept -> bool { return lhs.val == rhs.val; }

  friend constexpr auto operator!=(wrapped_char lhs, wrapped_char rhs) noexcept -> bool { return !(lhs == rhs); }

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wunused-function", "GCC8 sees this function as used (and required) even though Clang does not");
  friend auto operator>>(std::istream& istr, wrapped_char& wrapper) noexcept -> std::istream& {
    return istr >> wrapper.val;
  }
  ARENE_IGNORE_END();
#endif
};

// This class is for testing instantiation using a user-defined literal type.
class not_default_constructible {
 public:
  explicit constexpr not_default_constructible(int value)
      : value_{value} {}

  friend constexpr auto operator<(not_default_constructible lhs, not_default_constructible rhs) noexcept -> bool {
    // parasoft-begin-suppress AUTOSAR-M4_5_3-a "Just testing if comparison is possible, not using it for program logic"
    return lhs.value_ < rhs.value_;
    // parasoft-end-suppress AUTOSAR-M4_5_3-a
  }

  friend constexpr auto operator==(not_default_constructible lhs, not_default_constructible rhs) noexcept -> bool {
    return lhs.value_ == rhs.value_;
  }

  friend constexpr auto operator!=(not_default_constructible lhs, not_default_constructible rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wunused-function", "GCC8 sees this function as used (and required) even though Clang does not");
  friend auto operator>>(std::istream& istr, not_default_constructible& wrapper) noexcept -> std::istream& {
    return istr >> wrapper.value_;
  }
  ARENE_IGNORE_END();
#endif
 private:
  int value_;
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTS_TESTING_TYPES_HPP_
