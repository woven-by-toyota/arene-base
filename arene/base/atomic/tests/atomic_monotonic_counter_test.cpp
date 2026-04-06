// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/atomic/atomic_monotonic_counter.hpp"

#include <array>
#include <cstddef>
#include <limits>
#include <set>
#include <thread>
#include <type_traits>

#include <gtest/gtest.h>

#include "arene/base/synchronization/latch.hpp"

namespace {

using ::arene::base::atomic_monotonic_counter;

/// @test Verify that an @c atomic_monotonic_counter is initialized to zero by default.
TEST(AtomicCounter, DefaultConstructed) {
  auto const counter = atomic_monotonic_counter{};

  EXPECT_EQ(counter, 0ULL);
}

/// @test Verify that an @c atomic_monotonic_counter can be initialized with a specific value.
TEST(AtomicCounter, ConstructedWithValue) {
  auto const counter = atomic_monotonic_counter{42ULL};

  EXPECT_EQ(counter, 42ULL);
}

/// @test Verify that pre-increment operator returns the incremented value and updates the counter.
TEST(AtomicCounter, PreIncrement) {
  auto counter = atomic_monotonic_counter{42ULL};

  auto const result = ++counter;
  EXPECT_EQ(result, 43ULL);
  EXPECT_EQ(counter, 43ULL);
}

/// @test Verify that post-increment operator returns the original value and increments the counter.
TEST(AtomicCounter, PostIncrement) {
  auto counter = atomic_monotonic_counter{42ULL};

  auto const prev = counter++;
  EXPECT_EQ(prev, 42ULL);
  EXPECT_EQ(counter, 43ULL);
}

/// @test Verify that atomic_monotonic_counter supports implicit conversion to its underlying type.
TEST(AtomicCounter, ImplicitConversion) {
  auto const counter = atomic_monotonic_counter{42ULL};

  auto const value = counter;
  EXPECT_EQ(value, 42ULL);
}

/// @test Verify that explicit @c load() method returns the current counter value.
TEST(AtomicCounter, ExplicitLoad) {
  auto const counter = atomic_monotonic_counter{42ULL};

  static_assert(
      std::is_same<decltype(counter.load()), atomic_monotonic_counter::value_type>::value,
      "Expected load() to return the counter's value type."
  );
  EXPECT_EQ(counter.load(), 42ULL);
}

/// @test Verify that @c atomic_monotonic_counter explicitly wraps around.
TEST(AtomicCounter, WrapAroundBehavior) {
  auto counter = atomic_monotonic_counter{std::numeric_limits<atomic_monotonic_counter::value_type>::max() - 1};

  ++counter;
  EXPECT_EQ(counter.load(), std::numeric_limits<atomic_monotonic_counter::value_type>::max());

  ++counter;
  EXPECT_EQ(counter.load(), std::numeric_limits<atomic_monotonic_counter::value_type>::min());
}

/// @brief Run function @c func across @c num_threads different threads
/// @tparam F The function type
/// @param num_threads The number of threads to run
/// @param func The function to run
template <std::size_t N, class F>
auto run_in_threads(F const& func) {
  arene::base::latch start_latch{N};
  auto threads = std::array<std::thread, N>{};

  for (auto& thread : threads) {
    thread = std::thread{[&start_latch, &func]() {
      start_latch.arrive_and_wait();
      func();
    }};
  }

  for (auto& thread : threads) {
    thread.join();
  }
}

/// @test Verify that the @c atomic_monotonic_counter is thread-safe when multiple threads increment it concurrently.
TEST(AtomicCounter, MultipleIncrement) {
  constexpr auto num_threads = 10ULL;
  constexpr auto iterations = 10'000ULL;

  auto counter = atomic_monotonic_counter{};

  run_in_threads<num_threads>([&counter]() {
    for (auto i = 0U; i < iterations; ++i) {
      counter++;
    }
  });

  auto const value = counter;
  EXPECT_EQ(value, static_cast<std::uint64_t>(num_threads * iterations));
}

/// @test Verify that each @c atomic_monotonic_counter value is encountered just once while concurrently incrementing.
TEST(AtomicCounter, ValuesEncounteredOnce) {
  constexpr auto num_threads = 10ULL;
  constexpr auto iterations = 1'000'000ULL;
  constexpr auto expected_values = num_threads * iterations;

  auto counter = atomic_monotonic_counter{};

  struct test_context {
    std::set<std::uint64_t> encountered_values{};
    std::thread thread{};
  };

  arene::base::latch start_latch{num_threads};
  auto test_contexts = std::array<test_context, num_threads>{};
  for (auto& context : test_contexts) {
    auto& encountered_values = context.encountered_values;
    context.thread = std::thread{[&start_latch, &counter, &encountered_values]() {
      start_latch.arrive_and_wait();
      for (auto j = 0U; j < iterations; ++j) {
        auto const value = counter++;
        encountered_values.insert(value);
      }
    }};
  }

  for (auto& context : test_contexts) {
    context.thread.join();
  }

  // Merge together the encountered values from each thread.
  auto all_values = std::set<std::uint64_t>{};
  for (auto const& context : test_contexts) {
    auto const& encountered_values = context.encountered_values;
    all_values.insert(encountered_values.begin(), encountered_values.end());
  }

  // Each value should have been encountered exactly once.
  EXPECT_EQ(all_values.size(), expected_values);
  EXPECT_EQ(*all_values.begin(), 0U);
  EXPECT_EQ(*all_values.rbegin(), expected_values - 1);
}

}  // namespace
