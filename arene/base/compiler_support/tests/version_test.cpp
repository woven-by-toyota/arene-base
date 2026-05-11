// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file version_test.cpp
/// @brief Unit tests for version.hpp
///
#include "arene/base/compiler_support/version.hpp"

#include <ratio>

#include <gtest/gtest.h>

namespace {

/// @test The version variables must exist, and be usable in constant expressions
TEST(Version, CheckForConstexprVariables) {
  static_assert(arene::base::version_major >= 0, "variable must exist and be defined");
  static_assert(arene::base::version_minor >= 0, "variable must exist and be defined");
  static_assert(arene::base::version_patch >= 0, "variable must exist and be defined");
  static_assert(
      arene::base::version == arene::base::version_major * std::mega::num +
                                  arene::base::version_minor * std::milli::num + arene::base::version_minor,
      "variable must exist and be defined"
  );
}

}  // namespace
