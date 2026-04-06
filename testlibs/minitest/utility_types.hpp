// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_UTILITY_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_UTILITY_TYPES_HPP_

namespace testing {
// NOLINTNEXTLINE(google-runtime-int)
using line_number = unsigned long;

/// @brief Structure holding only a file and line, without test information
struct source_location {
  char const* file;
  line_number line;
};

/// @brief Structure to hold information about a test case
struct test_context {
  char const* suite_name = "";
  char const* test_case_name = "";
  char const* file_name = "";
  line_number line = 0;
  char const* type_param_string = "";
};

/// @brief Structure to hold strings for printing an assertion failure
struct assertion_strings {
  char const* arg1 = "";
  char const* description = "";
  char const* arg2 = "";
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_UTILITY_TYPES_HPP_
