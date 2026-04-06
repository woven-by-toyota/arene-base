// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/rotate.hpp"
#include "arene/base/array/array.hpp"

namespace {

/// @test `rotate` with invalid ranges is a precondition violation
TEST(
    RotateDeathTest,
    RotatingARandomAccessIteratorWhereMiddleIsBeforeFirstOrMiddleIsAfterLastIsAPreconditionViolation
) {
  auto content = arene::base::to_array({1, 2, 3, 4, 5});
  ASSERT_DEATH(arene::base::rotate(content.begin() + 1, content.begin(), content.end()), "Precondition");
  ASSERT_DEATH(arene::base::rotate(content.begin(), content.end(), content.end() - 1), "Precondition");
}

}  // namespace
