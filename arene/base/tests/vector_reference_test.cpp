// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file vector_reference_test.cpp
/// @brief Provides unit tests to validate export of content in vector_reference.hpp
///

#include "arene/base/inline_container/vector_reference.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `inline_vector_reference` is exported
TEST(VectorReference, InlineVectorReference) { using arene::base::inline_vector_reference; }

/// @test Ensure `vector_reference` is exported
TEST(VectorReference, VectorReference) { using arene::base::vector_reference; }

/// @test Ensure `const_vector_reference` is exported
TEST(VectorReference, ConstVectorReference) { using arene::base::const_vector_reference; }

/// @test Ensure `const_inline_vector_reference` is exported
TEST(VectorReference, ConstInlineVectorReference) { using arene::base::const_inline_vector_reference; }

/// @test Ensure `make_vector_reference` is exported
TEST(VectorReference, MakeVectorReference) { using arene::base::make_vector_reference; }

/// @test Ensure `make_const_vector_reference` is exported
TEST(VectorReference, MakeConstVectorReference) { using arene::base::make_const_vector_reference; }

}  // namespace
