// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/synchronization/barrier.hpp"

#include <array>
#include <chrono>
#include <future>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "arene/base/synchronization/latch.hpp"

namespace {

using namespace std::chrono_literals;

static_assert(!std::is_default_constructible<arene::base::barrier<>>::value, "Must not be default-constructible");
static_assert(!std::is_copy_constructible<arene::base::barrier<>>::value, "Must not be copyable");
static_assert(!std::is_copy_assignable<arene::base::barrier<>>::value, "Must not be copyable");
static_assert(!std::is_move_constructible<arene::base::barrier<>>::value, "Must not be movable");
static_assert(!std::is_move_assignable<arene::base::barrier<>>::value, "Must not be movable");

/// @test The maximum barrier count is the max value of a `ptrdiff_t`
TEST(Barrier, MaxCount) {
  static_assert(
      arene::base::barrier<>::max() == std::numeric_limits<std::ptrdiff_t>::max(),
      "Max value is ptrdiff max"
  );
  struct noop {
    void operator()() noexcept {}
  };
  ASSERT_EQ(arene::base::barrier<noop>::max(), std::numeric_limits<std::ptrdiff_t>::max());
}

/// @test `barrier` can be constructed with a count
TEST(Barrier, CanConstructWithCount) {
  static_assert(
      std::is_constructible<arene::base::barrier<>, std::ptrdiff_t>::value,
      "Must be constructible from count"
  );
  static_assert(
      std::is_nothrow_constructible<arene::base::barrier<>, std::ptrdiff_t>::value,
      "Must be nothrow constructible from count"
  );
  static_assert(
      !std::is_convertible<std::ptrdiff_t, arene::base::barrier<>>::value,
      "Must not be implicitly constructible from count"
  );

  arene::base::barrier<> const barrier(42);
  std::ignore = barrier;
}

/// @test Constructing a `barrier` with a negative count terminates the program with a precondiiton violation
TEST(BarrierDeathTest, NegativeCountIsPreconditionViolation) { ASSERT_DEATH(arene::base::barrier<>(-1), "count >= 0"); }

/// @test After constructing a barrier with a count of 2, `arrive_and_wait` blocks until `arrive` has been called on
/// another thread
TEST(Barrier, ArriveAndWaitBlocksUntilAllThreadsArrived) {
  arene::base::barrier<> barrier(2);

  auto future = std::async(std::launch::async, [&barrier] { barrier.arrive_and_wait(); });
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);

  std::ignore = barrier.arrive();

  EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);
}

/// @test After constructing a barrier with a count of 2, `wait` blocks until `arrive` has been called on another thread
TEST(Barrier, SeparateArriveAndWaitBlocksUntilAllThreadsArrived) {
  arene::base::barrier<> barrier(2);

  static_assert(
      std::is_same<decltype(barrier.arrive()), arene::base::barrier<>::arrival_token>::value,
      "arrive must return an arrival token"
  );

  auto future = std::async(std::launch::async, [&barrier] { barrier.wait(barrier.arrive()); });
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);

  std::ignore = barrier.arrive();

  EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);
}

/// @test After `arrive_and_wait` has woken because all threads have arrived, a subsequent call to `arrive_and_wait`
/// blocks again until other threads have arrived again
TEST(Barrier, BarrierCanBeReused) {
  arene::base::barrier<> barrier(2);
  arene::base::latch latch(1);

  std::promise<void> promise;
  auto arrived = promise.get_future();

  auto future = std::async(std::launch::async, [&barrier, &latch, &promise] {
    barrier.arrive_and_wait();
    promise.set_value();
    latch.wait();
    barrier.arrive_and_wait();
  });
  ASSERT_EQ(arrived.wait_for(500ms), std::future_status::timeout);

  std::ignore = barrier.arrive();
  EXPECT_EQ(arrived.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(arrived.wait_for(0ms) == std::future_status::ready);

  latch.count_down();
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);

  std::ignore = barrier.arrive();

  EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);
}

/// @test `arrive_and_wait` can be used repeatedly in a loop on two threads to synchronize those threads
TEST(Barrier, BarrierCanBeReusedInALoop) {
  arene::base::barrier<> barrier(2);

  constexpr auto loop_count = 100;

  auto future = std::async(std::launch::async, [&barrier] {
    for (unsigned i = 0; i < loop_count; ++i) {
      barrier.arrive_and_wait();
    }
  });
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);

  for (unsigned i = 0; i < (loop_count - 1); ++i) {
    barrier.arrive_and_wait();
  }
  EXPECT_EQ(future.wait_for(500ms), std::future_status::timeout);

  std::ignore = barrier.arrive();

  EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);
}

/// @test `arrive_and_wait` can be used repeatedly in a loop on 20 threads to synchronize those threads
TEST(Barrier, BarrierCanBeReusedInALoopWithMultipleWaitThreads) {
  constexpr auto thread_count = 20;
  arene::base::barrier<> barrier(thread_count + 1);

  constexpr auto loop_count = 100;

  std::array<std::future<void>, thread_count> threads;

  for (auto& future : threads) {
    future = std::async(std::launch::async, [&barrier] {
      for (unsigned i = 0; i < loop_count; ++i) {
        barrier.arrive_and_wait();
      }
    });
  }

  std::this_thread::sleep_for(500ms);
  for (auto& future : threads) {
    ASSERT_EQ(future.wait_for(0ms), std::future_status::timeout);
  }

  for (unsigned idx = 0; idx < (loop_count - 1); ++idx) {
    barrier.arrive_and_wait();
  }
  std::this_thread::sleep_for(500ms);
  for (auto& future : threads) {
    ASSERT_EQ(future.wait_for(0ms), std::future_status::timeout);
  }

  std::ignore = barrier.arrive();

  for (auto& future : threads) {
    EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
    ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);
  }
}

/// @test The completion function passed to the `barrier` constructor is invoked before `arrive_and_wait` returns.
TEST(Barrier, CompletionFunctionRunBeforeWaitReturns) {
  std::atomic<bool> t1_woken{false};
  std::atomic<bool> t2_woken{false};
  std::future<void> future1;
  std::future<void> future2;

  arene::base::latch latch(1);

  auto comp_future = [&t1_woken, &t2_woken, &latch] {
    std::this_thread::sleep_for(500ms);
    EXPECT_FALSE(t1_woken.load());
    EXPECT_FALSE(t2_woken.load());
    latch.wait();
  };

  static_assert(
      std::is_nothrow_constructible<
          arene::base::barrier<decltype(comp_future)>,
          std::ptrdiff_t,
          decltype(comp_future)>::value,
      "Must be nothrow constructible from count and future"
  );

  static_assert(
      !std::is_constructible<arene::base::barrier<decltype(comp_future)>, std::ptrdiff_t>::value,
      "Must not be constructible from just a count"
  );

  arene::base::barrier<decltype(comp_future)> barrier(2, comp_future);

  future1 = std::async(std::launch::async, [&barrier, &t1_woken] {
    barrier.arrive_and_wait();
    t1_woken = true;
  });
  future2 = std::async(std::launch::async, [&barrier, &t2_woken] {
    barrier.arrive_and_wait();
    t2_woken = true;
  });
  ASSERT_EQ(future1.wait_for(1s), std::future_status::timeout);
  ASSERT_EQ(future2.wait_for(0s), std::future_status::timeout);

  latch.count_down();

  EXPECT_EQ(future1.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future1.wait_for(0ms) == std::future_status::ready);
  EXPECT_EQ(future2.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future2.wait_for(0ms) == std::future_status::ready);
}

/// @test If one thread invokes `arrive_and_drop` then one fewer arrival is required to release waiters in the next
/// cycle
TEST(Barrier, AfterArriveAndDropOneFewerArrivalRequired) {
  arene::base::barrier<> barrier(4);

  std::promise<void> checkpoint1;
  auto arrived1 = checkpoint1.get_future();
  std::promise<void> checkpoint2;
  auto arrived2 = checkpoint2.get_future();
  std::promise<void> checkpoint3;
  auto arrived3 = checkpoint3.get_future();
  std::promise<void> checkpoint4;
  auto arrived4 = checkpoint4.get_future();
  std::promise<void> checkpoint5;
  auto arrived5 = checkpoint5.get_future();

  auto future = std::async(std::launch::async, [&barrier, &checkpoint1, &checkpoint2, &checkpoint3] {
    barrier.arrive_and_wait();
    checkpoint1.set_value();
    barrier.arrive_and_wait();
    checkpoint2.set_value();
    barrier.arrive_and_wait();
    checkpoint3.set_value();
    barrier.arrive_and_wait();
  });

  auto future2 = std::async(std::launch::async, [&barrier] { barrier.arrive_and_drop(); });
  auto future3 = std::async(std::launch::async, [&barrier, &checkpoint4, &checkpoint5] {
    barrier.arrive_and_wait();
    checkpoint4.set_value();
    barrier.arrive_and_wait();
    checkpoint5.set_value();
    barrier.arrive_and_drop();
  });
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);
  EXPECT_EQ(future2.wait_for(0ms), std::future_status::ready);
  ARENE_INVARIANT(future2.wait_for(0ms) == std::future_status::ready);
  ASSERT_EQ(future3.wait_for(0ms), std::future_status::timeout);
  ASSERT_EQ(arrived1.wait_for(0ms), std::future_status::timeout);
  ASSERT_EQ(arrived4.wait_for(0ms), std::future_status::timeout);

  barrier.arrive_and_wait();
  EXPECT_EQ(arrived1.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(arrived1.wait_for(0ms) == std::future_status::ready);
  EXPECT_EQ(arrived4.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(arrived4.wait_for(0ms) == std::future_status::ready);
  ASSERT_EQ(arrived2.wait_for(500ms), std::future_status::timeout);
  ASSERT_EQ(arrived5.wait_for(0ms), std::future_status::timeout);
  ASSERT_EQ(future.wait_for(0ms), std::future_status::timeout);
  ASSERT_EQ(future3.wait_for(0ms), std::future_status::timeout);

  barrier.arrive_and_wait();
  EXPECT_EQ(arrived2.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(arrived2.wait_for(0ms) == std::future_status::ready);
  EXPECT_EQ(arrived5.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(arrived5.wait_for(0ms) == std::future_status::ready);
  ASSERT_EQ(arrived3.wait_for(500ms), std::future_status::timeout);
  ASSERT_EQ(future.wait_for(0ms), std::future_status::timeout);
  EXPECT_EQ(future3.wait_for(0ms), std::future_status::ready);
  ARENE_INVARIANT(future3.wait_for(0ms) == std::future_status::ready);

  barrier.arrive_and_wait();
  EXPECT_EQ(arrived3.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(arrived3.wait_for(0ms) == std::future_status::ready);
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);

  std::ignore = barrier.arrive();
  EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);
}

/// @test `arrive` can be invoked with a count of arrivals
TEST(Barrier, CanArriveWithACount) {
  arene::base::barrier<> barrier(4);

  auto future = std::async(std::launch::async, [&barrier] { barrier.arrive_and_wait(); });
  ASSERT_EQ(future.wait_for(500ms), std::future_status::timeout);

  auto token = barrier.arrive(3);

  EXPECT_EQ(future.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future.wait_for(0ms) == std::future_status::ready);

  auto future2 =
      std::async(std::launch::async, [&barrier, tok = std::move(token)]() mutable { barrier.wait(std::move(tok)); });

  EXPECT_EQ(future2.wait_for(500ms), std::future_status::ready);
  ARENE_INVARIANT(future2.wait_for(0ms) == std::future_status::ready);
}

/// @test Invoking `arrive` with a negative count terminates the program with a precondiiton violation
TEST(BarrierDeathTest, NegativeCountForArriveIsPreconditionViolation) {
  arene::base::barrier<> barrier(4);

  ASSERT_DEATH(std::ignore = barrier.arrive(-1), "arrivals > 0");
}

/// @test Invoking `arrive` with zero terminates the program with a precondiiton violation
TEST(BarrierDeathTest, ZeroCountForArriveIsPreconditionViolation) {
  arene::base::barrier<> barrier(4);

  ASSERT_DEATH(std::ignore = barrier.arrive(0), "arrivals > 0");
}

/// @test Invoking `arrive` with a count larger than the remaining count terminates the program with a precondiiton
/// violation
TEST(BarrierDeathTest, TooLargeCountForArriveIsPreconditionViolation) {
  arene::base::barrier<> barrier(4);

  ASSERT_DEATH(std::ignore = barrier.arrive(5), "arrivals <= remaining_count");
}

/// @test The constructor of `barrier` throws if the move constructor of the supplied completion function throws
TEST(Barrier, BarrierConstructionThrowsIfCompletionFunctionMoveThrows) {
  struct error {};

  // NOLINTNEXTLINE(cppcoreguidelines-special-member-futuretions,hicpp-special-member-futuretions)
  class throwing_move {
   public:
    explicit throwing_move(unsigned count)
        : count_(count) {}

    // NOLINTNEXTLINE(bugprone-exception-escape,hicpp-noexcept-move,performance-noexcept-move-constructor)
    throwing_move(throwing_move&& other)
        : count_(other.count_ - 1) {
      if (count_ == 0) {
        // NOLINTNEXTLINE(hicpp-exception-baseclass)
        throw error{};
      }
    }
    // NOLINTNEXTLINE(bugprone-exception-escape,hicpp-noexcept-move,performance-noexcept-move-constructor)
    auto operator=(throwing_move&& other) -> throwing_move& {
      count_ = other.count_ - 1;
      if (count_ == 0) {
        // NOLINTNEXTLINE(hicpp-exception-baseclass)
        throw error{};
      }
      return *this;
    }

    ~throwing_move() = default;
    throwing_move(throwing_move const&) = delete;
    auto operator=(throwing_move const&) -> throwing_move& = delete;

    void operator()() const noexcept {}

   private:
    unsigned count_;
  };

  static_assert(
      !std::is_nothrow_constructible<arene::base::barrier<throwing_move>, int, throwing_move&&>::value,
      "Constructor should be able to throw"
  );
  ASSERT_THROW(arene::base::barrier<throwing_move>(1, throwing_move(1)), error);
  ASSERT_NO_THROW(arene::base::barrier<throwing_move>(1, throwing_move(2)));
}

/// @test The constructor of `barrier` without a completion function throws if the default constructor of the completion
/// function type throws
TEST(Barrier, BarrierConstructionThrowsIfCompletionFunctionDefaultConstructorThrows) {
  struct error {};

  // NOLINTNEXTLINE(cppcoreguidelines-special-member-futuretions,hicpp-special-member-futuretions)
  class throwing_construct {
   public:
    ARENE_NORETURN throwing_construct() {
      // NOLINTNEXTLINE(hicpp-exception-baseclass)
      throw error{};
    }

    void operator()() const noexcept {}
  };

  static_assert(
      !std::is_nothrow_constructible<arene::base::barrier<throwing_construct>, int>::value,
      "Constructor should be able to throw"
  );
  ASSERT_THROW(arene::base::barrier<throwing_construct>(1), error);
}
}  // namespace
