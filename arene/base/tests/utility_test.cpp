// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file utility_test.cpp
/// @brief Validates utility facilities are exported via utility.hpp
///
///
///

#include "arene/base/utility.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Validates that @c arene::base::as_const is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, AsConstIsExported) { using ::arene::base::as_const; }
/// @test Validates that @c arene::base::is_aligned is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, IsAlignedIsExported) { using ::arene::base::is_aligned; }
/// @test Validates that @c arene::base::align_floor is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, AlignFloorIsExported) { using ::arene::base::align_floor; }
/// @test Validates that @c arene::base::align_ceil is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, AlignCeilIsExported) { using ::arene::base::align_ceil; }
/// @test Validates that @c arene::base::alignment_offset is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, AlignmentOffsetIsExported) { using ::arene::base::alignment_offset; }

/// @test Validates that @c arene::base::bit_mask is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, BitMaskIsExported) { using ::arene::base::bit_mask; }

/// @test Validates that @c arene::base::forward_like is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, ForwardLikeIsExported) { using ::arene::base::forward_like; }

/// @test Validates that @c arene::base::in_place_t is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, InPlaceTIsExported) { using ::arene::base::in_place_t; }
/// @test Validates that @c arene::base::in_place_type_t is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, InPlaceTypeTIsExported) { using ::arene::base::in_place_type_t; }
/// @test Validates that @c arene::base::in_place_type is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, InPlaceTypeIsExported) { using ::arene::base::in_place_type; }
/// @test Validates that @c arene::base::in_place_index_t is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, InPlaceIndexTIsExported) { using ::arene::base::in_place_index_t; }
/// @test Validates that @c arene::base::in_place_index is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, InPlaceIndexIsExported) { using ::arene::base::in_place_index; }
/// @test Validates that @c arene::base::swap is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, SwapIsExported) { using ::arene::base::swap; }
/// @test Validates that @c arene::base::make_subrange is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, ToRangeIsExported) { using ::arene::base::make_subrange; }
/// @test Validates that @c arene::base::to_underlying is exported from @c "arene/base/utility.hpp" .
TEST(UtilityExports, ToUnderlyingIsExported) { using ::arene::base::to_underlying; }

}  // namespace
