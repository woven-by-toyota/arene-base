// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file algorithm_test.cpp
/// @brief Validates algorithm facilities are exported via algorithm.hpp
///
///
///

#include "arene/base/algorithm.hpp"

#include <gtest/gtest.h>

namespace {
/// @test `all_of` is declared
TEST(AlgorithmExports, AllOfIsExported) { using ::arene::base::all_of; }

/// @test `any_if` is declared
TEST(AlgorithmExports, AnyOfIsExported) { using ::arene::base::any_of; }

/// @test `clamp` is declared
TEST(AlgorithmExports, ClampIsExported) { using ::arene::base::clamp; }

/// @test `copy` is declared
TEST(AlgorithmExports, CopyIsExported) { using ::arene::base::copy; }

/// @test `copy_if` is declared
TEST(AlgorithmExports, CopyIfIsExported) { using ::arene::base::copy_if; }

/// @test `equal` is declared
TEST(AlgorithmExports, EqualIsExported) { using ::arene::base::equal; }

/// @test `fill` is declared
TEST(AlgorithmExports, FillIsExported) { using ::arene::base::fill; }

/// @test `find` is declared
TEST(AlgorithmExports, FindExported) { using ::arene::base::find; }

/// @test `find_if` is declared
TEST(AlgorithmExports, FindIfIsExported) { using ::arene::base::find_if; }

/// @test `iter_wap` is declared
TEST(AlgorithmExports, IterSwapIsExported) { using ::arene::base::iter_swap; }

/// @test `iota` is declared
TEST(AlgorithmExports, IotaIsExported) { using ::arene::base::iota; }

/// @test `inner_product` is declared
TEST(AlgorithmExports, InnerProductIsExported) { using ::arene::base::inner_product; }

/// @test `lexicographical_compare` is declared
TEST(AlgorithmExports, LexicographicalCompareIsExported) { using ::arene::base::lexicographical_compare; }

/// @test `lexicographical_compare_three_way` is declared
TEST(AlgorithmExports, LexicographicalCompareThreeWayIsExported) {
  using ::arene::base::lexicographical_compare_three_way;
}

/// @test `move` is declared
TEST(AlgorithmExports, MoveIsExported) { using ::arene::base::move; }

/// @test `move_backward` is declared
TEST(AlgorithmExports, MoveBackwardIsExported) { using ::arene::base::move_backward; }

/// @test `rotate` is declared
TEST(AlgorithmExports, RotateIsExported) { using ::arene::base::rotate; }

/// @test `swap_ranges` is declared
TEST(AlgorithmExports, SwapRangesIsExported) { using ::arene::base::swap_ranges; }

/// @test `transform` is declared
TEST(AlgorithmExports, TransformIsExported) { using ::arene::base::transform; }

/// @test `transform_reduce` is declared
TEST(AlgorithmExports, TransformReduceIsExported) { using ::arene::base::transform_reduce; }

}  // namespace
