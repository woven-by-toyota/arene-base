// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>

int main() {
  int values[5] = {};

  struct predicate {
    constexpr auto operator()(int) { return true; }
  };

  int const& start = values[0];
  int const& end = values[4];
  std::count_if(start, end, predicate{});
}
