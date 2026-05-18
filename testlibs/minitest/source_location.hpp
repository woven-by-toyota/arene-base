// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_SOURCE_LOCATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_SOURCE_LOCATION_HPP_

#include "testlibs/minitest/line_number.hpp"

namespace testing {

/// @brief Structure holding only a file and line, without test information
struct source_location {
  char const* file;
  line_number line;
};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_SOURCE_LOCATION_HPP_
