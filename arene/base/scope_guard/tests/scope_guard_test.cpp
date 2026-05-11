// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/scope_guard/scope_guard.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/detail/exceptions.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/move.hpp"

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
#include <stdexcept>
#endif

namespace {

using ::arene::base::on_scope_exit;
using ::arene::base::scope_guard;

/// @test `scope_guard` calls the provided function upon its destruction.
TEST(ScopeGuard, CallsFunctionOnDestruction) {
  bool called = false;
  auto func = [&called]() noexcept { called = true; };
  {
    scope_guard<decltype(func)> const call_on_exit{func};
    EXPECT_FALSE(called);
  }
  EXPECT_TRUE(called);
}

/// @test Moving `scope_guard` only calls the provided function once.
TEST(ScopeGuard, MovingScopeGuardOnlyCallsFunctionOnce) {
  int call_count = 0;
  auto func = [&call_count]() noexcept { ++call_count; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    EXPECT_EQ(call_count, 0);
    {
      auto nested_call_on_exit = std::move(call_on_exit);
      EXPECT_EQ(call_count, 0);
    }
    EXPECT_EQ(call_count, 1);
  }
  EXPECT_EQ(call_count, 1);
}

/// @test `cancel` prevents the provided function from being called upon scope destruction.
TEST(ScopeGuard, CancelPreventsFunctionFromBeingCalledDuringDtor) {
  bool called = false;
  auto func = [&called]() noexcept { called = true; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    EXPECT_FALSE(called);
    call_on_exit.cancel();
    EXPECT_FALSE(called);
  }
  EXPECT_FALSE(called);
}

/// @test `cancel` can be called multiple times without affecting behavior.
TEST(ScopeGuard, CancelCanBeCalledMultipleTimes) {
  bool called = false;
  auto func = [&called]() noexcept { called = true; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    EXPECT_FALSE(called);
    call_on_exit.cancel();
    call_on_exit.cancel();
    EXPECT_FALSE(called);
  }
  EXPECT_FALSE(called);
}

/// @test Moving a cancelled `scope_guard` doesn't call the provided function.
TEST(ScopeGuard, MovingCancelledDoesntCallFunction) {
  int call_count = 0;
  auto func = [&call_count]() noexcept { ++call_count; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    call_on_exit.cancel();
    EXPECT_EQ(call_count, 0);
    {
      auto nested_call_on_exit = std::move(call_on_exit);
      EXPECT_EQ(call_count, 0);
    }
    EXPECT_EQ(call_count, 0);
  }
  EXPECT_EQ(call_count, 0);
}

/// @test `scope_guard` calls the provided function upon exception throw.
TEST(ScopeGuard, InvokesFunctionWhenExitingViaException) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  bool called = false;
  auto func = [&called]() noexcept { called = true; };
  try {
    scope_guard<decltype(func)> const call_on_exit{func};
    throw std::runtime_error{""};
  } catch (std::runtime_error const&) {
    EXPECT_TRUE(called);
  }
  EXPECT_TRUE(called);
#else
  GTEST_SKIP() << "Cannot test exceptional case with exceptions disabled";
#endif
}

/// @test `invoke_now` immediately calls the provided function.
TEST(ScopeGuard, InvokeNow) {
  int call_count = 0;
  auto func = [&call_count]() noexcept { ++call_count; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    EXPECT_EQ(call_count, 0);
    call_on_exit.invoke_now();
    EXPECT_EQ(call_count, 1);
  }
  EXPECT_EQ(call_count, 1);
}

/// @test Calling `invoke_now` multiple times only calls the provided function once.
TEST(ScopeGuard, InvokeNowMultipleTimesOnlyCallsFunctionOnce) {
  int call_count = 0;
  auto func = [&call_count]() noexcept { ++call_count; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    EXPECT_EQ(call_count, 0);
    call_on_exit.invoke_now();
    EXPECT_EQ(call_count, 1);
    call_on_exit.invoke_now();
    EXPECT_EQ(call_count, 1);
    call_on_exit.invoke_now();
    EXPECT_EQ(call_count, 1);
  }
  EXPECT_EQ(call_count, 1);
}

/// @test `invoke_now` after `cancel` does not call the provided function.
TEST(ScopeGuard, InvokeNowAfterCancelDoesntInvoke) {
  int call_count = 0;
  auto func = [&call_count]() noexcept { ++call_count; };
  {
    scope_guard<decltype(func)> call_on_exit{func};
    EXPECT_EQ(call_count, 0);

    call_on_exit.cancel();

    EXPECT_EQ(call_count, 0);

    call_on_exit.invoke_now();

    EXPECT_EQ(call_count, 0);
  }
  EXPECT_EQ(call_count, 0);
}

/// @test `on_scope_exit` calls the provided function upon scope exit.
TEST(ScopeGuard, OnScopeExit) {
  int call_count = 0;
  {
    auto increment_on_exit = on_scope_exit([&call_count]() noexcept { ++call_count; });

    EXPECT_EQ(call_count, 0);
  }
  EXPECT_EQ(call_count, 1);
}

}  // namespace
