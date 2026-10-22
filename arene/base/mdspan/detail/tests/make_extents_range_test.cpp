// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/make_extents_range.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/make_range_from_indexable.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/iterator/detail/iter_concept.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::mdspan_detail::make_extents_range;
using arene::base::mdspan_detail::rank_end_fn;

/// @brief a minimal indexable exposing its rank through a non-static @c rank() member
///
/// @c extents::rank() is @c static @c constexpr, so through @c make_extents_range the @c rank_end_fn call is always
/// constant-folded and its body is never executed. This type carries the rank in a data member so a direct call to
/// @c rank_end_fn reads it at runtime, exercising the policy body itself.
class runtime_rank_view {
 public:
  /// @brief construct a view reporting @c rank dimensions
  /// @param rank the rank to report
  constexpr explicit runtime_rank_view(std::size_t const rank) noexcept
      : rank_{rank} {}

  /// @brief report the rank
  /// @return the stored rank
  constexpr auto rank() const noexcept -> std::size_t { return rank_; }

 private:
  /// @brief the reported rank
  std::size_t rank_{};
};

/// @test the extents view yields input iterators because @c extent() returns by value but the iterator concept is
/// random access
TEST(MakeExtentsRangeTest, YieldsInputIterators) {
  using extents_type = arene::base::dextents<std::size_t, 3>;
  auto const exts = extents_type{4, 5, 6};
  auto const view = make_extents_range(exts);
  using iterator_type = decltype(view.begin());

  ::testing::StaticAssertTypeEq<iterator_type::iterator_category, std::input_iterator_tag>();
  ::testing::
      StaticAssertTypeEq<arene::base::iterator_detail::iter_concept_t<iterator_type>, std::random_access_iterator_tag>(
      );
}

/// @test extents view iterators have members associated with random access
CONSTEXPR_TEST(MakeExtentsRangeTest, IteratorHasRandomAccesMembers) {
  using extents_type = arene::base::dextents<std::size_t, 3>;
  auto const exts = extents_type{4, 5, 6};
  auto const view = make_extents_range(exts);

  CONSTEXPR_ASSERT_EQ(view.begin()[0], exts.extent(0));
  CONSTEXPR_ASSERT_EQ(view.begin()[1], exts.extent(1));
  CONSTEXPR_ASSERT_EQ(view.begin()[2], exts.extent(2));

  CONSTEXPR_ASSERT(view.begin() < view.end());
}

/// @test extents view has members associated with a random access range
CONSTEXPR_TEST(MakeExtentsRangeTest, HasRandomAccessRangeMembers) {
  using extents_type = arene::base::dextents<std::size_t, 3>;
  auto const exts = extents_type{4, 5, 6};
  auto const view = make_extents_range(exts);

  CONSTEXPR_ASSERT_EQ(view[0], exts.extent(0));
  CONSTEXPR_ASSERT_EQ(view[1], exts.extent(1));
  CONSTEXPR_ASSERT_EQ(view[2], exts.extent(2));
}

/// @test an extents is iterated as a range of its per-dimension extents
CONSTEXPR_TEST(MakeExtentsRangeTest, ExtentsYieldsEachExtent) {
  using extents_type = arene::base::dextents<std::size_t, 3>;
  auto const exts = extents_type{4, 5, 6};
  auto const view = make_extents_range(exts);

  CONSTEXPR_ASSERT((view.end() - view.begin()) == 3);

  auto index = std::size_t{0};
  for (auto extent : view) {
    CONSTEXPR_ASSERT(extent == exts.extent(index));
    ++index;
  }
  CONSTEXPR_ASSERT(index == 3);
}

/// @test a rank-0 extents produces an empty range
CONSTEXPR_TEST(MakeExtentsRangeTest, RankZeroExtentsIsEmpty) {
  using extents_type = arene::base::extents<std::size_t>;
  auto const exts = extents_type{};
  auto const view = make_extents_range(exts);

  CONSTEXPR_ASSERT(view.begin() == view.end());
}

/// @test Given indexables with runtime and static ranks, when the rank end policy is called, then it reports each
/// one-past-last index via the @c rank() member.
CONSTEXPR_TEST(MakeExtentsRangeTest, RankEndPolicyReportsRank) {
  using dynamic_extents_type = arene::base::dextents<std::size_t, 3>;
  using matrix_extents_type = arene::base::extents<std::size_t, 3, 2>;
  using scalar_extents_type = arene::base::extents<std::size_t>;

  CONSTEXPR_ASSERT(rank_end_fn{}(runtime_rank_view{3}) == 3U);
  CONSTEXPR_ASSERT(rank_end_fn{}(dynamic_extents_type{4, 5, 6}) == 3U);
  CONSTEXPR_ASSERT(rank_end_fn{}(matrix_extents_type{}) == 2U);
  CONSTEXPR_ASSERT(rank_end_fn{}(scalar_extents_type{}) == 0U);
}

}  // namespace
