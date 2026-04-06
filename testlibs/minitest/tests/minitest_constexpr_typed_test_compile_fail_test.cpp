// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

using types_for_testing = ::testing::Types<int>;

template <typename T>
class Suite : public testing::Test {};

TYPED_TEST_SUITE(Suite, types_for_testing, );

auto non_constexpr_function() -> void {}

CONSTEXPR_TYPED_TEST(Suite, FailsIfNotConstexprInvocable) { non_constexpr_function(); }
