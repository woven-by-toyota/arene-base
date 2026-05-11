// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file result_test.cpp
/// @brief Tests export of expected symbols from arene/base/result.hpp
///
#include "arene/base/result.hpp"

#include <gtest/gtest.h>

namespace {
/// @test @c arene::base::in_place_value_t is exported from @c "arene/base/result.hpp"
TEST(ResultExports, InPlaceValueT) { using ::arene::base::in_place_value_t; }
/// @test @c arene::base::in_place_value is exported from @c "arene/base/result.hpp"
TEST(ResultExports, InPlaceValue) { using ::arene::base::in_place_value; }

/// @test @c arene::base::in_place_error_t is exported from @c "arene/base/result.hpp"
TEST(ResultExports, InPlaceErrorT) { using ::arene::base::in_place_error_t; }
/// @test @c arene::base::in_place_error is exported from @c "arene/base/result.hpp"
TEST(ResultExports, InPlaceError) { using ::arene::base::in_place_error; }

/// @test @c arene::base::result is exported from @c "arene/base/result.hpp"
TEST(ResultExports, Result) { using ::arene::base::result; }

/// @test @c arene::base::expand_result is exported from @c "arene/base/result.hpp"
TEST(ResultExports, ExpandResult) { using ::arene::base::expand_result; }

/// @test @c arene::base::value_result_t is exported from @c "arene/base/result.hpp"
TEST(ResultExports, ValueResultT) { using ::arene::base::value_result_t; }
/// @test @c arene::base::value_result is exported from @c "arene/base/result.hpp"
TEST(ResultExports, ValueResult) { using ::arene::base::value_result; }

/// @test @c arene::base::error_result_t is exported from @c "arene/base/result.hpp"
TEST(ResultExports, ErrorResultT) { using ::arene::base::error_result_t; }
/// @test @c arene::base::error_result is exported from @c "arene/base/result.hpp"
TEST(ResultExports, ErrorResult) { using ::arene::base::error_result; }

}  // namespace
