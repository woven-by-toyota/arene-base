// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

/// @file add_death_test.cpp
/// Provides tests for linalg::add. Additionally, used as a template with
/// expand_template to provide tests for linalg::subtract.
///

#include <gtest/gtest.h>

#include "arene/base/linalg/add.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
// IWYU pragma: no_include "arene/base/functional/bind_back.hpp"
// IWYU pragma: no_include "arene/base/linalg/elementwise_binary_transform.hpp"
// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

namespace {
/// @test Calling @c add with @c mdspan instantiations whose runtime extents disagree is a precondition violation
TEST(LinalgAddDeathTests, MismatchedExtentsIsPreconditionViolation) {
  using arene::base::linalg_testing::dynamic_test_array;
  namespace linalg = arene::base::linalg;

  ASSERT_DEATH(
      linalg::add(
          dynamic_test_array<float, int, 5, 6, 2>{}.mdspan(),
          dynamic_test_array<float, int, 5, 6, 2>{}.mdspan(),
          dynamic_test_array<float, int, 5, 6, 3>{}.mdspan()
      ),
      "Precondition"
  );

  ASSERT_DEATH(
      linalg::add(
          dynamic_test_array<float, int, 5, 6, 3>{}.mdspan(),
          dynamic_test_array<float, int, 5, 6, 3>{}.mdspan(),
          dynamic_test_array<float, int, 5, 6, 2>{}.mdspan()
      ),
      "Precondition"
  );

  ASSERT_DEATH(
      linalg::add(
          dynamic_test_array<float, int, 5, 6, 3>{}.mdspan(),
          dynamic_test_array<float, int, 5, 6, 2>{}.mdspan(),
          dynamic_test_array<float, int, 5, 6, 3>{}.mdspan()
      ),
      "Precondition"
  );
}
}  // namespace
