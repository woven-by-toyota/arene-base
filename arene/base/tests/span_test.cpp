// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file span_test.cpp
/// @brief Validates that expected symbols are exported from span.hpp
///

#include "arene/base/span.hpp"

#include <gtest/gtest.h>

namespace {
/// @test `span` is declared
TEST(Functional, Span) { using ::arene::base::span; }

}  // namespace
