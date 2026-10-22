// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"
#include "testlibs/minitest/minitest_output.hpp"

auto main() -> int {
  ::testing::console_output out{};

  if (!::testing::test_base::run_tests(out)) {
    ::testing::minitest::exit_program(1);
  }
}
