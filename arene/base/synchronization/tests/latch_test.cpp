// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/synchronization/latch.hpp"

#include <chrono>
#include <future>
#include <type_traits>

#include <gtest/gtest.h>

namespace {

using namespace std::chrono_literals;

static_assert(!std::is_copy_constructible<arene::base::latch>::value, "Must not be copy constructible");
static_assert(!std::is_copy_assignable<arene::base::latch>::value, "Must not be copy assignable");
static_assert(!std::is_move_constructible<arene::base::latch>::value, "Must not be move constructible");
static_assert(!std::is_move_assignable<arene::base::latch>::value, "Must not be move assignable");

/// @test The max count for `latch` is the max value of `ptrdiff_t`
TEST(Latch, MaxCount) {
  static_assert(arene::base::latch::max() == std::numeric_limits<std::ptrdiff_t>::max(), "Max value is ptrdiff max");
  ASSERT_EQ(arene::base::latch::max(), std::numeric_limits<std::ptrdiff_t>::max());
}

/// @test A `latch` can be constructed with a count
TEST(Latch, CanConstructLatchWithACount) {
  static_assert(!std::is_default_constructible<arene::base::latch>::value, "Not default constructible");
  static_assert(std::is_constructible<arene::base::latch, std::ptrdiff_t>::value, "Constructible from a count");
  static_assert(
      !std::is_convertible<std::ptrdiff_t, arene::base::latch>::value,
      "Not implicitly constructible from a count"
  );
  arene::base::latch const latch(42);
  static_cast<void>(latch);
}

/// @test Constructing a `latch` with a negative count terminates the process with a precondition violation
TEST(LatchDeathTest, ConstructingLatchWithNegativeCountIsPreconditionViolation) {
  ASSERT_DEATH(arene::base::latch(-1), "count >= 0");
}

/// @test If the count passed to the `latch` constructor is zero then `try_wait` succeeds immediately
TEST(Latch, IfInitialCountIsZeroTryWaitSucceeds) {
  arene::base::latch const latch(0);
  ASSERT_TRUE(latch.try_wait());
  static_assert(noexcept(latch.try_wait()), "Must be noexcept");
  static_assert(std::is_same<decltype(latch.try_wait()), bool>::value, "Must return bool");
}

/// @test If the count passed to the `latch` constructor is non-zero then `try_wait` fails if `count_down` not called
TEST(Latch, IfInitialCountIsNonZeroTryWaitFails) {
  arene::base::latch const latch(1);
  ASSERT_FALSE(latch.try_wait());
}

/// @test If the count passed to the `latch` constructor is one then `try_wait` succeeds if `count_down` is called
TEST(Latch, AfterCountDownTryWaitSucceeds) {
  arene::base::latch latch(1);
  latch.count_down();
  ASSERT_TRUE(latch.try_wait());
  static_assert(noexcept(latch.try_wait()), "Must be noexcept");
  static_assert(std::is_same<decltype(latch.try_wait()), bool>::value, "Must return bool");
}

/// @test If the count passed to the `latch` constructor is non-zero then `try_wait` fails until `count_down` is called
/// the required number of times, and then succeeds
TEST(Latch, TryWaitFailsUntilCountDownToZero) {
  constexpr unsigned count = 42U;
  arene::base::latch latch(count);
  for (unsigned idx = 0; idx < count; ++idx) {
    ASSERT_FALSE(latch.try_wait());
    latch.count_down();
  }
  ASSERT_TRUE(latch.try_wait());
}

/// @test Invoking `count_down` on a `latch` that already has a count of zero terminates the process with a precondition
/// violation
TEST(LatchDeathTest, CannotCountDownBeyondZero) {
  arene::base::latch latch(0);
  ASSERT_DEATH(latch.count_down(), "\\(update >= 0\\) && \\(update <= count_\\)");
}

/// @test Invoking `count_down` on a `latch` with a count that exceeds the remaining count of the latch terminates the
/// process with a precondition violation
TEST(LatchDeathTest, CannotCountDownBeyondZeroWithBigSteps) {
  arene::base::latch latch(5);
  ASSERT_DEATH(latch.count_down(6), "\\(update >= 0\\) && \\(update <= count_\\)");
}

/// @test Invoking `count_down` on a `latch` with a negative count terminates the process with a precondition
/// violation
TEST(LatchDeathTest, CannotCountDownWithNegativeStep) {
  arene::base::latch latch(5);
  ASSERT_DEATH(latch.count_down(-1), "\\(update >= 0\\) && \\(update <= count_\\)");
}

/// @test `count_down` can be invoked with a count, which reduces the latch counter by the specified value
TEST(Latch, CanCountDownBigSteps) {
  arene::base::latch latch(10);
  latch.count_down(5);
  ASSERT_FALSE(latch.try_wait());
  latch.count_down();
  ASSERT_FALSE(latch.try_wait());
  latch.count_down();
  ASSERT_FALSE(latch.try_wait());
  latch.count_down();
  ASSERT_FALSE(latch.try_wait());
  latch.count_down();
  ASSERT_FALSE(latch.try_wait());
  latch.count_down();
  ASSERT_TRUE(latch.try_wait());
}

/// @test If `latch` is constructed with a zero count, then `wait` does not block
TEST(Latch, WaitReturnsImmediatelyIfCountIsZero) {
  arene::base::latch const latch(0);
  arene::base::latch const& clatch{latch};

  auto func = std::async(std::launch::async, [&clatch] { clatch.wait(); });
  ASSERT_EQ(func.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(func.wait_for(0ms) == std::future_status::ready);
}

/// @test If `latch` is constructed with a count of one, then `wait` blocks until `count_down` is called
TEST(Latch, WaitBlocksUntilCountIsZero) {
  arene::base::latch latch(1);
  arene::base::latch const& clatch{latch};

  auto func = std::async(std::launch::async, [&clatch] { clatch.wait(); });
  ASSERT_EQ(func.wait_for(500ms), std::future_status::timeout);

  latch.count_down();

  ASSERT_EQ(func.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(func.wait_for(0ms) == std::future_status::ready);
}

/// @test If `latch` is constructed with a count of one, then multiple threads can invoke `wait` concurrently, and all
/// will block until `count_down` is called
TEST(Latch, MultipleWaitingThreadsBlocksUntilCountIsZero) {
  arene::base::latch latch(1);
  arene::base::latch const& clatch{latch};

  auto func = std::async(std::launch::async, [&clatch] { clatch.wait(); });
  auto func2 = std::async(std::launch::async, [&clatch] { clatch.wait(); });
  ASSERT_EQ(func.wait_for(500ms), std::future_status::timeout);
  ASSERT_EQ(func2.wait_for(0ms), std::future_status::timeout);

  latch.count_down();

  ASSERT_EQ(func.wait_for(500ms), std::future_status::ready);
  ASSERT_EQ(func2.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(func.wait_for(0ms) == std::future_status::ready);
  ARENE_INVARIANT(func2.wait_for(0ms) == std::future_status::ready);
}

/// @test If `latch` is constructed with a count of two, then `arrive_and_wait` blocks until `count_down` is called
TEST(Latch, ArriveAndWaitBlocksUntilCountReachesZero) {
  arene::base::latch latch(2);

  auto func = std::async(std::launch::async, [&latch] { latch.arrive_and_wait(); });
  ASSERT_EQ(func.wait_for(500ms), std::future_status::timeout);

  latch.count_down();

  ASSERT_EQ(func.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(func.wait_for(0ms) == std::future_status::ready);
}

/// @test `arrive_and_wait` can be invoked with a count, which decrements the latch counter by the specified number
TEST(Latch, ArriveAndWaitCanTakeCount) {
  arene::base::latch latch(5);

  auto func = std::async(std::launch::async, [&latch] { latch.arrive_and_wait(3); });
  ASSERT_EQ(func.wait_for(500ms), std::future_status::timeout);

  latch.count_down(2);

  ASSERT_EQ(func.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(func.wait_for(0ms) == std::future_status::ready);
}

}  // namespace
