// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"
#include "testlibs/minitest/minitest_output.hpp"

auto main() -> int {
  ::testing::xml_file_and_console_output out{};

  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  if (auto const* path = ::testing::minitest::get_env_var("XML_OUTPUT_FILE")) {
    out.open_file(path);
  }

  if (!::testing::test_base::run_tests(out)) {
    ::testing::minitest::exit_program(1);
  }
}
