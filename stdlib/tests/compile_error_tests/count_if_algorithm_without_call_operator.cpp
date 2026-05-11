// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>

int main() {
  int values[5] = {};

  struct predicate {};
  std::count_if(std::begin(values), std::end(values), predicate{});
}
