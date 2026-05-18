// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Example usage of arene::base::atomic_monotonic_counter
#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>

#include "arene/base/atomic.hpp"

using arene::base::atomic_monotonic_counter;

// [basic_usage]
// Simple increment example
auto basic_usage() -> void {
  atomic_monotonic_counter counter;

  // Pre-increment - prints "New value: 1"
  uint64_t new_value = ++counter;
  std::cout << "New value: " << new_value << std::endl;

  // Post-increment - prints "Previous value: 1"
  uint64_t prev_value = counter++;
  std::cout << "Previous value: " << prev_value << std::endl;

  // Read the value using implicit conversion - prints "Value: 7"
  uint64_t value = counter;
  std::cout << "Value: " << value << std::endl;

  // Read the value using explicit load - prints "Same value: 7"
  uint64_t same_value = counter.load();
  std::cout << "Same value: " << same_value << std::endl;
}
// [basic_usage]

// [concurrent_updates]
// Example with concurrent updates
auto concurrent_updates() -> void {
  atomic_monotonic_counter shared_counter;
  constexpr int num_threads = 4;
  constexpr int iterations = 1000;

  // Create multiple threads that increment the counter
  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back([&shared_counter]() {
      for (int j = 0; j < iterations; ++j) {
        ++shared_counter;
      }
    });
  }

  // Wait for all threads to complete
  for (auto& t : threads) {
    t.join();
  }

  // The final value should be num_threads * iterations
  uint64_t final_value = shared_counter;
  printf("Final counter value: %lu (expected: %d)\n", final_value, num_threads * iterations);
}
// [concurrent_updates]

// [counter_as_event_tracker]
// Using atomic_monotonic_counter to track events
auto counter_as_event_tracker() -> void {
  atomic_monotonic_counter requests(0);
  atomic_monotonic_counter completed(0);
  atomic_monotonic_counter errors(0);

  // Simulate processing requests
  auto process_request = [&](int id) -> void {
    ++requests;

    // Simulate work
    bool success = (id % 10 != 0);  // 10% error rate

    if (success) {
      ++completed;
    } else {
      ++errors;
    }
  };

  // Process some requests
  for (int i = 0; i < 100; ++i) {
    process_request(i);
  }

  // Report statistics
  printf("Requests: %lu\n", requests.load());
  printf("Completed: %lu\n", completed.load());
  printf("Errors: %lu\n", errors.load());
}
// [counter_as_event_tracker]
