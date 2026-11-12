// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_ASSERTION_STRINGS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_ASSERTION_STRINGS_HPP_

namespace testing {

/// @brief Structure to hold strings for printing an assertion failure
struct assertion_strings {
  char const* arg1 = "";
  char const* description = "";
  char const* arg2 = "";
};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_ASSERTION_STRINGS_HPP_
