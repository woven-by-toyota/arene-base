// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/synchronization/manual_reset_event.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <thread>

#include <gtest/gtest.h>

namespace {

/// @test Concurrent calls to `reset` on a signalled event all complete without blocking forever.
///
/// Before the fix, `reset` read the signalled flag and stored it as two separate steps, so multiple threads could
/// all observe the event as signalled and all then wait on the single relay token left by `signal`, leaving every
/// caller but one blocked forever. The race window is only a few instructions wide, so this test releases all
/// racing threads from a tight spin-wait (rather than e.g. a `latch`, whose own OS-level wakeup jitter was large
/// enough in practice to mask the race entirely) and repeats across many iterations to make a reintroduced race
/// show up reliably instead of intermittently.
TEST(ManualResetEvent, ConcurrentResetDoesNotHang) {
  constexpr std::size_t iterations = 2000U;
  constexpr std::size_t thread_count = 16U;

  for (std::size_t iter = 0U; (iter < iterations) && !HasFailure(); ++iter) {
    arene::base::manual_reset_event event;
    event.signal();

    std::atomic<bool> go{false};
    std::atomic<std::size_t> ready{0U};
    std::array<std::atomic<bool>, thread_count> done{};
    std::array<std::thread, thread_count> threads;

    for (std::size_t idx = 0U; idx < thread_count; ++idx) {
      threads[idx] = std::thread([&event, &go, &ready, &done, idx] {
        ready.fetch_add(1U, std::memory_order_relaxed);
        while (!go.load(std::memory_order_acquire)) {
        }
        event.reset();
        done[idx].store(true, std::memory_order_relaxed);
      });
    }

    while (ready.load(std::memory_order_relaxed) < thread_count) {
    }
    go.store(true, std::memory_order_release);

    auto const deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    for (std::size_t idx = 0U; idx < thread_count; ++idx) {
      while (!done[idx].load(std::memory_order_relaxed) && (std::chrono::steady_clock::now() < deadline)) {
      }
    }

    // Join every finished thread and detach any still stuck in `reset`, so a hung thread (the bug under test)
    // reports as a clean assertion failure below instead of letting `~thread` call `std::terminate`.
    for (std::size_t idx = 0U; idx < thread_count; ++idx) {
      if (done[idx].load(std::memory_order_relaxed)) {
        threads[idx].join();
      } else {
        threads[idx].detach();
      }
    }

    for (std::size_t idx = 0U; idx < thread_count; ++idx) {
      EXPECT_TRUE(done[idx].load(std::memory_order_relaxed))
          << "iteration " << iter << ", thread " << idx << " blocked forever in reset()";
    }
  }
}

}  // namespace
