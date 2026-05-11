// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <numeric>

constexpr int values[5] = {};

int main() {
  struct adder {
    constexpr auto operator()(int, int) { return 0; }
  };

  int const& start = values[0];
  int const& end = values[4];

  static_assert(std::accumulate(&start, &end, 0) == 0, "Correct sum");
  static_assert(std::accumulate(&start, &end, 0, adder{}) == 0, "Correct sum");
}
