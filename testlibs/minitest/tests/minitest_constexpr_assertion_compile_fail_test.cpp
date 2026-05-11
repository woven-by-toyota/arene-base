// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

CONSTEXPR_TEST(Suite, OneNotEqualToTwo) { ASSERT_EQ(1, 2); }
