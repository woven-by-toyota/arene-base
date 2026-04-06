// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>

constexpr int values[5] = {};

int main() {
  struct predicate {
    constexpr auto operator()(int) { return true; }
  };

  int const& start = values[0];
  int const& end = values[4];

  static_assert(!std::none_of(&start, &end, predicate{}), "All true");
}
