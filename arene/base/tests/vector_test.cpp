// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file vector_test.cpp
/// @brief Provides unit tests to validate export of content in vector.hpp
///

#include "arene/base/inline_container/vector.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `inline_vector` is exported
TEST(Vector, InlineVector) { using arene::base::inline_vector; }

}  // namespace
