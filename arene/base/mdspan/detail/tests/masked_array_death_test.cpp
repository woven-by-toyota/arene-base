// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/array/to_array.hpp"
#include "arene/base/mdspan/detail/masked_array.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"

namespace {

using arene::base::mdspan_detail::masked_array;

/// @test accessing an index that is not mapped is a precondition violation
TEST(MdspanDetailMaskedArrayDeathTest, UnmappedIndex) {
  ASSERT_DEATH((masked_array<0>{arene::base::array<bool, 0>{}}[0]), "Precondition");
  ASSERT_DEATH((masked_array<1>{arene::base::to_array({false})}[0]), "Precondition");
  ASSERT_DEATH((masked_array<3>{arene::base::to_array({true, false, true})}[1]), "Precondition");
  ASSERT_DEATH((masked_array<3>{arene::base::to_array({true, false, true})}[3]), "Precondition");

  // these instantiations below are mostly for coverage
  ASSERT_DEATH((masked_array<1>{arene::base::array<bool, 1>{}}[0]), "Precondition");
  ASSERT_DEATH((masked_array<2>{arene::base::array<bool, 2>{}}[0]), "Precondition");
  ASSERT_DEATH((masked_array<4>{arene::base::array<bool, 4>{}}[0]), "Precondition");
  ASSERT_DEATH((masked_array<6>{arene::base::array<bool, 6>{}}[0]), "Precondition");
  ASSERT_DEATH((masked_array<7>{arene::base::array<bool, 7>{}}[0]), "Precondition");
  ASSERT_DEATH((masked_array<14>{arene::base::array<bool, 14>{}}[0]), "Precondition");

  ASSERT_DEATH((masked_array<1>{arene::base::array<bool, 1>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<2>{arene::base::array<bool, 2>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<3>{arene::base::array<bool, 3>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<4>{arene::base::array<bool, 4>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<4>{arene::base::array<bool, 4>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<5>{arene::base::array<bool, 5>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<6>{arene::base::array<bool, 6>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<7>{arene::base::array<bool, 7>{}}[20]), "Precondition");
  ASSERT_DEATH((masked_array<14>{arene::base::array<bool, 14>{}}[20]), "Precondition");

  std::ignore = masked_array<1>{arene::base::array<bool, 1>{true}}[0];
  std::ignore = masked_array<2>{arene::base::array<bool, 2>{true}}[0];
  std::ignore = masked_array<3>{arene::base::array<bool, 3>{true}}[0];
  std::ignore = masked_array<4>{arene::base::array<bool, 4>{true}}[0];
  std::ignore = masked_array<5>{arene::base::array<bool, 5>{true}}[0];
  std::ignore = masked_array<6>{arene::base::array<bool, 6>{true}}[0];
  std::ignore = masked_array<7>{arene::base::array<bool, 7>{true}}[0];
  std::ignore = masked_array<14>{arene::base::array<bool, 14>{true}}[0];
}

}  // namespace
