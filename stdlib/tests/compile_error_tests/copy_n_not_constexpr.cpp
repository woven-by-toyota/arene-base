// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>

constexpr int values[5] = {1, 2, 3, 4, 5};

constexpr bool do_copy() {
  int dest[5] = {};
  std::copy_n(&values[0], 5, &dest[0]);
  return dest[0] == 1;
}

int main() { static_assert(do_copy(), "Copied OK"); }
