// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file ranges_test.cpp
/// @brief Provides unit tests to validate export of content in ranges.hpp
///

#include "arene/base/ranges.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `begin` is declared
TEST(RangesExports, Begin) { using arene::base::ranges::begin; }
/// @test `end` is declared
TEST(RangesExports, End) { using arene::base::ranges::end; }
/// @test `iterator_t` is declared
TEST(RangesExports, IteratorT) { using arene::base::ranges::iterator_t; }
/// @test `sentinel_t` is declared
TEST(RangesExports, SentinelT) { using arene::base::ranges::sentinel_t; }

/// @test `enable_borrowed_range` is declared
TEST(RangesExports, EnableBorrowedRange) { using arene::base::ranges::enable_borrowed_range; }

/// @test `disable_sized_range` is declared
TEST(RangesExports, DisableSizedRange) { using arene::base::ranges::disable_sized_range; }
/// @test `disable_sized_range_v` is declared
TEST(RangesExports, DisableSizedRangeV) { using arene::base::ranges::disable_sized_range_v; }

/// @test `is_range` is declared
TEST(RangesExports, IsRange) { using arene::base::ranges::is_range; }
/// @test `is_range_v` is declared
TEST(RangesExports, IsRangeV) { using arene::base::ranges::is_range_v; }

/// @test `size` is declared
TEST(RangesExports, Size) { using arene::base::ranges::size; }

/// @test `is_borrowed_range` is declared
TEST(RangesExports, IsBorrowedRange) { using arene::base::ranges::is_borrowed_range; }
/// @test `is_borrowed_range_v` is declared
TEST(RangesExports, IsBorrowedRangeV) { using arene::base::ranges::is_borrowed_range_v; }

/// @test `dangling` is declared
TEST(RangesExports, Dangling) { using arene::base::ranges::dangling; }

/// @test `borrowed_iterator_t` is declared
TEST(RangesExports, BorrowedIteratorT) { using arene::base::ranges::borrowed_iterator_t; }

}  // namespace
