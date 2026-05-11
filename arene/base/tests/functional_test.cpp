// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file functional_test.cpp
/// @brief Validates that expected symbols are exported from functional.hpp
///

#include "arene/base/functional.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Ensure `invoke` is available
TEST(Functional, Invoke) { using ::arene::base::invoke; }

/// @test Ensure `function_ref` is available
TEST(Functional, FunctionRef) { using ::arene::base::function_ref; }

/// @test Ensure `inline_function` is available
TEST(Functional, InlineFunction) { using ::arene::base::inline_function; }

/// @test Ensure `bind_front` is available
TEST(Functional, BindFront) { using ::arene::base::bind_front; }

/// @test Ensure `bind_back` is available
TEST(Functional, BindBack) { using ::arene::base::bind_back; }

/// @test `bind_overloads` is declared
TEST(Functional, BindOverloads) { using ::arene::base::bind_overloads; }

/// @test Ensure `identity` is available
TEST(Functional, identity) { using ::arene::base::identity; }

/// @test Ensure `not_fn` is available
TEST(Functional, NotFn) { using ::arene::base::not_fn; }

}  // namespace
