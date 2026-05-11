// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/map_rank.hpp"

#include <gmock/gmock.h>

#include "arene/base/mdspan/full_extent.hpp"

namespace {
using keep = arene::base::full_extent_t;
using drop = int;

/// @test inverse_map_rank_impl provides the inverse mapping of map_rank
/// @note We directly call @c inverse_map_rank_impl to ensure it has full
/// coverage
TEST(MdspanDetail, InverseMapRank) {
  using arene::base::mdspan_detail::map_rank_detail::inverse_map_rank_impl;
  using testing::ElementsAreArray;

  EXPECT_THAT(  //
      (inverse_map_rank_impl<keep, keep, keep, keep, keep>()),
      ElementsAreArray({0U, 1U, 2U, 3U, 4U})
  );

  EXPECT_THAT(  //
      (inverse_map_rank_impl<keep, drop, keep, drop, keep>()),
      ElementsAreArray({0U, 2U, 4U})
  );

  EXPECT_THAT(  //
      (inverse_map_rank_impl<keep, keep, keep, drop, drop>()),
      ElementsAreArray({0U, 1U, 2U})
  );

  EXPECT_THAT(  //
      (inverse_map_rank_impl<drop, drop, keep, drop, keep>()),
      ElementsAreArray({2U, 4U})
  );

  EXPECT_THAT(  //
      (inverse_map_rank_impl<drop, drop, drop, keep, drop>()),
      ElementsAreArray({3U})
  );

  unsigned empty_range{};
  EXPECT_THAT(  //
      (inverse_map_rank_impl<drop, drop, drop, drop, drop>()),
      ElementsAreArray(&empty_range, &empty_range)
  );
}

}  // namespace
