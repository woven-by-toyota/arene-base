// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/masked_array.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/algorithm/transform.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/detail/count.hpp"
#include "arene/base/mdspan/detail/map_rank.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

// NOLINTBEGIN(readability-identifier-length)

// an alias meaning "no-entry" in the mapping
//
// we use `-1` as the unmapped marker since GCC8 will not implicitly convert
// arguments and we want to use a C-array to deduce the size.
constexpr auto _ = -1;

namespace test {

// NOLINTBEGIN(modernize-use-trailing-return-type)
MATCHER_P2(
    ElementsAreMaskedArray,
    begin,
    end,
    "range elements provided by [begin, end) are equal to the masked array"
) {
  std::size_t index{};
  for (auto iter = begin; iter != end; ++iter) {
    auto i = index++;
    auto const& elem = *iter;
    if ((elem >= 0) != arg.is_valid(i)) {
      return false;
    }
    if ((elem >= 0) && (static_cast<std::size_t>(elem) != arg[i])) {
      return false;
    }
  }

  using arene::base::mdspan_detail::count;
  return static_cast<std::size_t>((end - begin) - count(begin, end, _)) == arg.codomain_size();
}
// NOLINTEND(modernize-use-trailing-return-type)

template <std::size_t N>
auto assert_dynamic_indices_are(  //
    arene::base::array<std::size_t, N> static_extents,
    int const (&expected)[N]  // NOLINT(hicpp-avoid-c-arrays)
) -> void {
  auto const rank_mask = [&static_extents] {
    auto mask = arene::base::array<bool, N>{};

    arene::base::transform(  //
        std::begin(static_extents),
        std::end(static_extents),
        mask.begin(),
        [](auto const ext) { return ext == arene::base::dynamic_extent; }
    );

    return mask;
  }();

  auto const indices = arene::base::mdspan_detail::masked_array<N>{rank_mask};

  ASSERT_THAT(indices, (ElementsAreMaskedArray(std::begin(expected), std::end(expected))));
}

template <class... Slices, std::size_t N>
auto assert_map_rank_is(int const (&expected)[N]) -> void {  // NOLINT(hicpp-avoid-c-arrays)
  ASSERT_THAT(
      (arene::base::mdspan_detail::map_rank<Slices...>),
      (ElementsAreMaskedArray(std::begin(expected), std::end(expected)))
  );
}

}  // namespace test

namespace {
/// @test Given a zero-rank mask, when a mapping is constructed, then its codomain is empty and every index is invalid.
CONSTEXPR_TEST(MdspanDetailMaskedArrayTest, ZeroRankHasNoValidIndices) {
  auto const mapping = arene::base::mdspan_detail::masked_array<0>{arene::base::array<bool, 0>{}};
  CONSTEXPR_ASSERT_EQ(mapping.codomain_size(), 0U);
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(0));
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(arene::base::dynamic_extent));
}

/// @test Given a mask with no retained ranks, when a mapping is constructed, then its codomain is empty.
CONSTEXPR_TEST(MdspanDetailMaskedArrayTest, DroppedRanksHaveNoValidIndices) {
  auto const mapping = arene::base::mdspan_detail::masked_array<3>{arene::base::array<bool, 3>{}};
  CONSTEXPR_ASSERT_EQ(mapping.codomain_size(), 0U);
  for (std::size_t i{}; i != 3; ++i) {
    CONSTEXPR_ASSERT_FALSE(mapping.is_valid(i));
  }
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(3));
}

/// @test Given a mask retaining every rank, when a mapping is constructed, then it maps each rank to itself.
CONSTEXPR_TEST(MdspanDetailMaskedArrayTest, RetainedRanksMapToThemselves) {
  auto const mapping = arene::base::mdspan_detail::masked_array<3>{arene::base::array<bool, 3>{true, true, true}};
  CONSTEXPR_ASSERT_EQ(mapping.codomain_size(), 3U);
  for (std::size_t i{}; i != 3; ++i) {
    CONSTEXPR_ASSERT(mapping.is_valid(i));
    CONSTEXPR_ASSERT_EQ(mapping[i], i);
  }
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(3));
}

/// @test Given a mixed mask, when a mapping is constructed, then retained ranks map to consecutive output indices.
CONSTEXPR_TEST(MdspanDetailMaskedArrayTest, MixedMaskMapsRetainedRanksConsecutively) {
  auto const mapping =
      arene::base::mdspan_detail::masked_array<5>{arene::base::array<bool, 5>{false, true, false, true, false}};
  CONSTEXPR_ASSERT_EQ(mapping.codomain_size(), 2U);
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(0));
  CONSTEXPR_ASSERT(mapping.is_valid(1));
  CONSTEXPR_ASSERT_EQ(mapping[1], 0U);
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(2));
  CONSTEXPR_ASSERT(mapping.is_valid(3));
  CONSTEXPR_ASSERT_EQ(mapping[3], 1U);
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(4));
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(5));
  CONSTEXPR_ASSERT_FALSE(mapping.is_valid(arene::base::dynamic_extent));
}

/// @test Additional checks for the @c determine_dynamic_indices internal helper function to ensure it has
/// full coverage
TEST(MdspanDetail, DetermineDynamicIndicesReturnsTheExpectedValues) {
  using arene::base::dynamic_extent;

  test::assert_dynamic_indices_are(  //
      {dynamic_extent},
      {0}
  );
  test::assert_dynamic_indices_are(  //
      {1, 2, 3, dynamic_extent, 42},
      {_, _, _, 0, _}
  );
  test::assert_dynamic_indices_are(  //
      {1, dynamic_extent, 42},
      {_, 0, _}
  );
  test::assert_dynamic_indices_are(
      {1, dynamic_extent, 42, dynamic_extent, 42, dynamic_extent, 42},
      {_, 0, _, 1, _, 2, _}
  );
  test::assert_dynamic_indices_are(
      {1, dynamic_extent, 42, dynamic_extent, 42, dynamic_extent, 42, dynamic_extent, 42, 43, 47, 42, 42, dynamic_extent
      },
      {_, 0, _, 1, _, 2, _, 3, _, _, _, _, _, 4}
  );
  test::assert_dynamic_indices_are(
      {
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
          dynamic_extent,
      },
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}
  );
  test::assert_dynamic_indices_are(
      {
          5,
          dynamic_extent,
      },
      {_, 0}
  );
  test::assert_dynamic_indices_are(  //
      {dynamic_extent, dynamic_extent},
      {0, 1}
  );
  test::assert_dynamic_indices_are(  //
      {dynamic_extent, dynamic_extent, dynamic_extent, dynamic_extent, dynamic_extent, 42},
      {0, 1, 2, 3, 4, _}
  );
  test::assert_dynamic_indices_are(  //
      {dynamic_extent, dynamic_extent, dynamic_extent, 42},
      {0, 1, 2, _}
  );
}

template <class T>
class MdspanDetail : public testing::Test {};

TYPED_TEST_SUITE(MdspanDetail, test::index_types, );

/// @test @c map_rank drops ranks if slices are convertible to the specified @c index_type
TYPED_TEST(MdspanDetail, MapRank) {
  using Index = TypeParam;

  using arene::base::dynamic_extent;
  using arene::base::full_extent_t;
  using slice = arene::base::extent_slice<Index>;

  struct user_defined {};

  test::assert_map_rank_is<user_defined>(  //
      {_}
  );
  test::assert_map_rank_is<arene::base::range_slice<Index>>(  //
      {_}
  );
  test::assert_map_rank_is<user_defined, user_defined, user_defined>(  //
      {_, _, _}
  );
  test::assert_map_rank_is<full_extent_t, full_extent_t, full_extent_t>(  //
      {0, 1, 2}
  );
  test::assert_map_rank_is<slice, slice, slice>(  //
      {0, 1, 2}
  );

  test::assert_map_rank_is<Index, Index, Index>(  //
      {_, _, _}
  );

  test::assert_map_rank_is<float, std::integral_constant<int, 0>, std::size_t>(  //
      {_, _, _}
  );

  test::assert_map_rank_is<int, user_defined, Index>(  //
      {_, _, _}
  );

  test::assert_map_rank_is<int, full_extent_t, Index>(  //
      {_, 0, _}
  );

  test::assert_map_rank_is<user_defined, Index, user_defined>(  //
      {_, _, _}
  );

  test::assert_map_rank_is<full_extent_t, Index, full_extent_t>(  //
      {0, _, 1}
  );
}

}  // namespace

// NOLINTEND(readability-identifier-length)
