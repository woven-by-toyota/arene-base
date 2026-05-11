// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>

class target {};

class source {};

int main() {
  source sources[5] = {};
  target targets[5] = {};

  std::move(&sources[0], &sources[2], &targets[0]);
}
