// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries.hpp"
#include "testlibs/minitest/minitest.hpp"

struct some_exception {};
struct other_exception {};

void throw_some_exception(bool do_throw) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  if (do_throw) {
    throw some_exception();
  }
#else
  static_cast<void>(do_throw);
#endif
}

void does_not_throw() {}

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
TEST(AssertThrow, ThrowingExpectedException) { ASSERT_THROW(throw_some_exception(true), some_exception); }

TEST(AssertThrow, ThrowingWrongException) { ASSERT_THROW(throw_some_exception(true), other_exception); }

TEST(AssertThrow, NotThrowingExceptionWhenExpected) { ASSERT_THROW(does_not_throw(), other_exception); }
#endif

TEST(AssertNothrow, NotThrowingException) { ASSERT_NO_THROW(does_not_throw()); }

TEST(AssertNothrow, ThrowingExceptionWhenNotExpected) { ASSERT_NO_THROW(throw_some_exception(true)); }

TEST(AssertNothrow, ThrowingExceptionOutsideAssert) { throw_some_exception(true); }
