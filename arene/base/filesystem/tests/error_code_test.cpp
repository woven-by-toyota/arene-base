// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/error_code.hpp"

#include <cerrno>
#include <cstring>
#include <system_error>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/string_view.hpp"

namespace {

/// @test `from_errno` returns an `error_code` such that `value` returns the value of `errno` prior to the call.
TEST(ErrorCode, CanConstructFromErrno) {
  int const value = 123;
  errno = value;

  auto const code = arene::base::filesystem::error_code::from_errno();
  static_assert(noexcept(arene::base::filesystem::error_code::from_errno()), "Must be noexcept");
  static_assert(std::is_same<decltype(code), arene::base::filesystem::error_code const>::value, "Right type");

  ASSERT_EQ(errno, 0);
  ASSERT_EQ(code.value(), value);
  static_assert(noexcept(code.value()), "Must be noexcept");
}

/// @test A default-constructed `error_code` has a `value` of zero
TEST(ErrorCode, DefaultConstructedErrorCodeHasZeroValue) {
  arene::base::filesystem::error_code const code{};

  ASSERT_EQ(code.value(), 0);
}

/// @test A default-constructed `error_code` cast to `bool` evaluates as `false`
TEST(ErrorCode, DefaultConstructedEvaluatesAsFalse) {
  arene::base::filesystem::error_code const code{};

  ASSERT_FALSE(code);
  static_assert(noexcept(static_cast<bool>(code)), "Must be noexcept");
}

/// @test Invoking `value` on an `error_code` constructed with a specific value returns the value passed to the
/// constructor
TEST(ErrorCode, CanConstructWithExplicitValue) {
  int const value = 42;
  arene::base::filesystem::error_code const code{value};
  static_assert(noexcept(arene::base::filesystem::error_code{value}), "Must be noexcept");

  ASSERT_EQ(code.value(), value);
}

/// @test An `error_code` constructed with a non-zero value evaluates as `true` when cast to `bool`
TEST(ErrorCode, ErrorCodeWithValueEvaluatesAsTrue) {
  int const value = 42;
  arene::base::filesystem::error_code const code{value};
  ASSERT_TRUE(code);
}

/// @test After invoking `clear` on an `error_code`, `value` returns zero
TEST(ErrorCode, AfterClearValueIsZero) {
  int const value = 123;
  arene::base::filesystem::error_code code{value};
  code.clear();
  static_assert(noexcept(code.clear()), "Must be noexcept");
  ASSERT_EQ(code.value(), 0);
}

/// @test Invoking `message` on an `error_code` returns a string equal to the result of invoking `strerror` with the
/// error value passed to thc constructor
TEST(ErrorCode, CanGetMessage) {
  arene::base::filesystem::error_code code{ERANGE};
  static_assert(noexcept(code.message()), "Must be noexcept");
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_STREQ(code.message().c_str(), strerror(code.value()));
  code = arene::base::filesystem::error_code(EINVAL);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_STREQ(code.message().c_str(), strerror(code.value()));
}

/// @test Invoking `message` on an `error_code` constructed from an error without a corresponding value in `errno.h`
/// returns an empty string
TEST(ErrorCode, CanGetMessageForUnknownError) {
  arene::base::filesystem::error_code const code{-12345};
  ASSERT_STRNE(code.message().c_str(), "");
}

/// @test Invoking `throw_error` throws a `std::system_error` with a category of `std::system_category`, and a value
/// equal to the value of the `error_code`
TEST(ErrorCode, CanThrowSystemError) {
  arene::base::filesystem::error_code const code{ERANGE};
  ASSERT_THROW(code.throw_error(), std::system_error);

  try {
    code.throw_error();
  } catch (std::system_error const& e) {
    ASSERT_EQ(&e.code().category(), &std::system_category());
    ASSERT_EQ(e.code().value(), code.value());
  }
}

/// @test Invoking `throw_error_with_prefix` throws a `std::system_error` with a message that includes the specified
/// prefix
TEST(ErrorCode, CanThrowSystemErrorWithMessage) {
  arene::base::filesystem::error_code const code{ERANGE};
  char const* msg = "In a galaxy far far away";
  try {
    code.throw_error_with_prefix(arene::base::null_terminated_string_view{msg});
  } catch (std::system_error const& exception) {
    ASSERT_EQ(exception.code().value(), ERANGE);

    arene::base::string_view const error_message(exception.what());

    ASSERT_TRUE(error_message.find(msg) != arene::base::string_view::npos);
  }
}

}  // namespace
