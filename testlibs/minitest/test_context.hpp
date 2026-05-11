#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TEST_CONTEXT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TEST_CONTEXT_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "testlibs/minitest/line_number.hpp"

namespace testing {

/// @brief Structure to hold information about a test case
struct test_context {
  char const* suite_name = "";
  char const* test_case_name = "";
  char const* file_name = "";
  line_number line = 0;
  char const* type_param_string = "";
};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TEST_CONTEXT_HPP_
