// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/layout_constant_diagonal.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test @c layout_constant_diagonal is a layout mapping policy
TEST(LayoutConstantDiagonal, IsLayoutMappingPolicy) {
  ASSERT_TRUE((arene::base::is_layout_mapping_policy_v<
               arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>>));
  ASSERT_TRUE((arene::base::is_layout_mapping_policy_v<
               arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>>));
  ASSERT_TRUE((arene::base::is_layout_mapping_policy_v<
               arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>>));
  ASSERT_TRUE((arene::base::is_layout_mapping_policy_v<arene::base::linalg::layout_constant_diagonal<
                   arene::base::extents<std::int32_t, 42, arene::base::dynamic_extent>>>));
}

/// @test @c layout_constant_diagonal mappings are valid layout mappings
TEST(LayoutConstantDiagonal, MappingsAreValidLayoutMappings) {
  ASSERT_TRUE((arene::base::is_layout_mapping_v<arene::base::linalg::layout_constant_diagonal<
                   arene::base::extents<std::size_t>>::mapping<arene::base::extents<std::size_t>>>));
  ASSERT_TRUE((arene::base::is_layout_mapping_v<arene::base::linalg::layout_constant_diagonal<
                   arene::base::extents<std::size_t, 0>>::mapping<arene::base::dextents<std::size_t, 1>>>));
  ASSERT_TRUE((arene::base::is_layout_mapping_v<arene::base::linalg::layout_constant_diagonal<
                   arene::base::extents<std::size_t, 0, 0>>::mapping<arene::base::dextents<std::size_t, 2>>>));
  ASSERT_TRUE((arene::base::is_layout_mapping_v<arene::base::linalg::layout_constant_diagonal<
                   arene::base::extents<std::int32_t, 42, arene::base::dynamic_extent>>::
                                                    mapping<arene::base::dextents<std::size_t, 2>>>));
}

/// @test The @c required_span_size of a @c mapping for a @c layout_constant_diagonal is always 1 for rank zero extents
CONSTEXPR_TEST(LayoutConstantDiagonal, RequiredSpanSizeIs1ForRankZeroExtents) {
  CONSTEXPR_ASSERT_EQ(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
          arene::base::extents<std::size_t>>{}
          .required_span_size(),
      1U
  );

  CONSTEXPR_ASSERT_EQ(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t>>::mapping<
          arene::base::extents<std::int32_t>>{}
          .required_span_size(),
      1
  );
}

/// @test The @c is_unique member function of a @c mapping for a @c layout_constant_diagonal always returns @c true for
/// rank zero extents
CONSTEXPR_TEST(LayoutConstantDiagonal, IsUniqueIsTrueForRankZeroExtents) {
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
                       arene::base::extents<std::size_t>>{}
                       .is_unique());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t>>::mapping<
                       arene::base::extents<std::int32_t>>{}
                       .is_unique());
}

/// @test The @c required_span_size of a @c mapping for a @c layout_constant_diagonal is always 2 for non-empty rank one
/// extents
CONSTEXPR_TEST(LayoutConstantDiagonal, RequiredSpanSizeIs2ForNonEmptyRankOneExtents) {
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
           arene::base::extents<std::size_t, 3>>{}
           .required_span_size()),
      2U
  );

  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 1>>::mapping<
           arene::base::extents<std::int32_t, 3>>{}
           .required_span_size()),
      2
  );

  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 1>>::mapping<
           arene::base::extents<std::int32_t, 42>>{}
           .required_span_size()),
      2
  );
}

/// @test The @c is_unique member function of a @c mapping for a @c layout_constant_diagonal always returns @c false for
/// non-empty rank one extents
CONSTEXPR_TEST(LayoutConstantDiagonal, IsUniqueIsFalseForNonEmptyRankOneExtents) {
  CONSTEXPR_ASSERT_FALSE((arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                              arene::base::extents<std::size_t, 3>>{}
                              .is_unique()));

  CONSTEXPR_ASSERT_FALSE((arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 1>>::mapping<
                              arene::base::extents<std::int32_t, 3>>{}
                              .is_unique()));

  CONSTEXPR_ASSERT_FALSE((arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 1>>::mapping<
                              arene::base::extents<std::int32_t, 42>>{}
                              .is_unique()));
}

/// @test The @c required_span_size of a @c mapping for a @c layout_constant_diagonal is always 0 for empty rank one
/// extents
CONSTEXPR_TEST(LayoutConstantDiagonal, RequiredSpanSizeIs0ForEmptyRankOneExtents) {
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
           arene::base::extents<std::size_t, 0>>{}
           .required_span_size()),
      0U
  );

  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 1>>::mapping<
           arene::base::extents<std::int32_t, 0>>{}
           .required_span_size()),
      0
  );

  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 1>>::mapping<
           arene::base::extents<std::int32_t, 0>>{}
           .required_span_size()),
      0
  );
}

/// @test The @c is_unique member function of a @c mapping for a @c layout_constant_diagonal always returns @c true for
/// empty rank one extents
CONSTEXPR_TEST(LayoutConstantDiagonal, IsUniqueIsTrueForEmptyRankOneExtents) {
  CONSTEXPR_ASSERT((arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                        arene::base::extents<std::size_t, 0>>{}
                        .is_unique()));

  CONSTEXPR_ASSERT((arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 1>>::mapping<
                        arene::base::extents<std::int32_t, 0>>{}
                        .is_unique()));

  CONSTEXPR_ASSERT((arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 1>>::mapping<
                        arene::base::extents<std::int32_t, 0>>{}
                        .is_unique()));
}

template <std::size_t Offset, std::size_t Extent>
struct rank_1_diagonal_offsets_and_extents {
  static constexpr std::size_t offset = Offset;
  static constexpr std::size_t extent = Extent;
};

using rank1_diagonal_types = ::testing::Types<
    rank_1_diagonal_offsets_and_extents<0, 4>,
    rank_1_diagonal_offsets_and_extents<0, 0>,
    rank_1_diagonal_offsets_and_extents<1, 1>,
    rank_1_diagonal_offsets_and_extents<1, 0>,
    rank_1_diagonal_offsets_and_extents<2, 0>,
    rank_1_diagonal_offsets_and_extents<2, 2>>;

template <class OffsetAndExtent>
class LayoutConstantDiagonalRank1Test : public ::testing::Test {};

TYPED_TEST_SUITE(LayoutConstantDiagonalRank1Test, rank1_diagonal_types, );

/// @test The @c required_span_size of a @c mapping for a @c layout_constant_diagonal is 2 for non-empty dynamic rank
/// one extents, 0 for empty dynamic rank one extents
CONSTEXPR_TYPED_TEST(LayoutConstantDiagonalRank1Test, RequiredSpanSizeIs0Or2ForDynamicRankOneExtents) {
  constexpr auto extent = TypeParam::extent;
  constexpr auto offset = TypeParam::offset;
  using dynamic_extents_1 = arene::base::extents<std::size_t, arene::base::dynamic_extent>;
  using dynamic_extents_2 = arene::base::extents<std::int32_t, arene::base::dynamic_extent>;

  bool const empty = (extent == 0U);
  std::size_t const e1span = empty ? 0U : 2U;
  std::int32_t const e2span = empty ? 0 : 2;

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::size_t, offset>>::template mapping<dynamic_extents_1>{dynamic_extents_1{extent}
      }.required_span_size()),
      e1span
  );
  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::size_t, offset>>::template mapping<dynamic_extents_2>{dynamic_extents_2{extent}
      }.required_span_size()),
      e2span
  );
  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::int32_t, offset>>::template mapping<dynamic_extents_1>{dynamic_extents_1{extent}
      }.required_span_size()),
      e1span
  );
  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::int32_t, offset>>::template mapping<dynamic_extents_2>{dynamic_extents_2{extent}
      }.required_span_size()),
      e2span
  );
}

template <std::size_t Offset1, std::size_t Offset2, std::size_t Extent1, std::size_t Extent2>
struct rank_2_diagonal_offsets_and_extents {
  static constexpr std::size_t offset1 = Offset1;
  static constexpr std::size_t offset2 = Offset2;
  static constexpr std::size_t extent1 = Extent1;
  static constexpr std::size_t extent2 = Extent2;
};

using rank2_diagonal_types = ::testing::Types<
    rank_2_diagonal_offsets_and_extents<0, 0, 4, 6>,
    rank_2_diagonal_offsets_and_extents<0, 0, 4, 0>,
    rank_2_diagonal_offsets_and_extents<1, 0, 10, 20>,
    rank_2_diagonal_offsets_and_extents<1, 0, 0, 20>,
    rank_2_diagonal_offsets_and_extents<0, 1, 0, 20>,
    rank_2_diagonal_offsets_and_extents<0, 1, 10, 20>,
    rank_2_diagonal_offsets_and_extents<0, 1, 20, 0>,
    rank_2_diagonal_offsets_and_extents<2, 5, 0, 10>,
    rank_2_diagonal_offsets_and_extents<2, 5, 3, 10>,
    rank_2_diagonal_offsets_and_extents<2, 5, 10, 0>>;

template <class OffsetAndExtent>
class LayoutConstantDiagonalRank2Test : public ::testing::Test {};

TYPED_TEST_SUITE(LayoutConstantDiagonalRank2Test, rank2_diagonal_types, );

/// @test The @c required_span_size of a @c mapping for a @c layout_constant_diagonal is 2 for non-empty dynamic rank
/// two extents, 0 for empty dynamic rank two extents
CONSTEXPR_TYPED_TEST(LayoutConstantDiagonalRank2Test, RequiredSpanSizeIs0Or2ForDynamicRankTwoExtents) {
  constexpr auto extent1 = TypeParam::extent1;
  constexpr auto extent2 = TypeParam::extent2;
  constexpr auto offset1 = TypeParam::offset1;
  constexpr auto offset2 = TypeParam::offset2;
  using dynamic_extents_1 = arene::base::dextents<std::size_t, 2>;
  using dynamic_extents_2 = arene::base::dextents<std::int32_t, 2>;

  bool const empty = (extent1 == 0U) || (extent2 == 0U);
  std::size_t const e1span = empty ? 0U : 2U;
  std::int32_t const e2span = empty ? 0 : 2;

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::size_t, offset1, offset2>>::template mapping<dynamic_extents_1>{
          dynamic_extents_1{extent1, extent2}
      }.required_span_size()),
      e1span
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::size_t, offset1, offset2>>::template mapping<dynamic_extents_2>{
          dynamic_extents_2{extent1, extent2}
      }.required_span_size()),
      e2span
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::int32_t, offset1, offset2>>::template mapping<dynamic_extents_1>{
          dynamic_extents_1{extent1, extent2}
      }.required_span_size()),
      e1span
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<
           arene::base::extents<std::int32_t, offset1, offset2>>::template mapping<dynamic_extents_2>{
          dynamic_extents_2{extent1, extent2}
      }.required_span_size()),
      e2span
  );
}

/// @test constant diagonal layouts are exhaustive if they are rank 0
CONSTEXPR_TEST(LayoutConstantDiagonal, Rank0LayoutsAreExhaustive) {
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
                       arene::base::extents<std::size_t>>{}
                       .is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t>>::mapping<
                       arene::base::extents<std::int32_t>>{}
                       .is_exhaustive());
}

/// @test constant diagonal layouts are exhaustive if they are empty
CONSTEXPR_TEST(LayoutConstantDiagonal, EmptyLayoutsAreExhaustive) {
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                       arene::base::extents<std::size_t, 0>>{}
                       .is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                       arene::base::extents<std::size_t, arene::base::dynamic_extent>>{
      arene::base::extents<std::size_t, arene::base::dynamic_extent>{0}
  }.is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 0>>::mapping<
                       arene::base::extents<std::size_t, arene::base::dynamic_extent>>{
      arene::base::extents<std::size_t, arene::base::dynamic_extent>{0}
  }.is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
                       arene::base::extents<std::size_t, 0, 3>>{}
                       .is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 10, 10>>::mapping<
                       arene::base::extents<std::int32_t, 2, 0>>{}
                       .is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 10, 10>>::mapping<
                       arene::base::extents<std::int32_t, 2, arene::base::dynamic_extent>>{
      arene::base::extents<std::int32_t, 2, arene::base::dynamic_extent>{0}
  }.is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 3>>::mapping<
                       arene::base::dextents<std::size_t, 2>>{arene::base::dextents<std::size_t, 2>{0, 2}
  }.is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 3>>::mapping<
                       arene::base::dextents<std::size_t, 2>>{arene::base::dextents<std::size_t, 2>{2, 0}
  }.is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 10, 10>>::mapping<
                       arene::base::extents<std::int32_t, arene::base::dynamic_extent, 2>>{
      arene::base::extents<std::int32_t, arene::base::dynamic_extent, 2>{0}
  }.is_exhaustive());
  CONSTEXPR_ASSERT(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10, 10>>::mapping<
          arene::base::extents<std::size_t, arene::base::dynamic_extent, 2, 5>>{
          arene::base::extents<std::size_t, arene::base::dynamic_extent, 2, 5>{0}
      }.is_exhaustive()
  );
  CONSTEXPR_ASSERT(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10, 10>>::mapping<
          arene::base::extents<std::size_t, 2, arene::base::dynamic_extent, 5>>{
          arene::base::extents<std::size_t, 2, arene::base::dynamic_extent, 5>{0}
      }.is_exhaustive()
  );
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 0, 0, 0>>::mapping<
                       arene::base::extents<std::int32_t, arene::base::dynamic_extent, 1, 1>>{
      arene::base::extents<std::int32_t, arene::base::dynamic_extent, 1, 1>{0}
  }.is_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 0, 0, 0>>::mapping<
                       arene::base::extents<std::int32_t, 1, arene::base::dynamic_extent, 1>>{
      arene::base::extents<std::int32_t, 1, arene::base::dynamic_extent, 1>{0}
  }.is_exhaustive());
}

/// @test constant diagonal layouts are not exhaustive if the largest dimension has extent 1
CONSTEXPR_TEST(LayoutConstantDiagonal, LayoutsAreNotExhaustiveIfLargestDimensionHasExtent1) {
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                             arene::base::extents<std::size_t, 1>>{}
                             .is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<
                             arene::base::extents<std::size_t, 0, 0>>::mapping<arene::base::extents<std::size_t, 1, 1>>{
  }
                             .is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0, 0>>::mapping<
          arene::base::extents<std::size_t, 1, 1, 1>>{}
          .is_exhaustive()
  );

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                             arene::base::extents<std::size_t, arene::base::dynamic_extent>>{
      arene::base::extents<std::size_t, arene::base::dynamic_extent>{1}
  }.is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 0>>::mapping<
                             arene::base::extents<std::size_t, arene::base::dynamic_extent>>{
      arene::base::extents<std::size_t, arene::base::dynamic_extent>{1}
  }.is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 5>>::mapping<
                             arene::base::extents<std::size_t, arene::base::dynamic_extent>>{
      arene::base::extents<std::size_t, arene::base::dynamic_extent>{3}
  }.is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 5>>::mapping<
                             arene::base::extents<std::size_t, arene::base::dynamic_extent>>{
      arene::base::extents<std::size_t, arene::base::dynamic_extent>{3}
  }.is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
          arene::base::extents<std::size_t, arene::base::dynamic_extent, 1>>{
          arene::base::extents<std::size_t, arene::base::dynamic_extent, 1>{1}
      }.is_exhaustive()
  );

  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0, 0>>::mapping<
          arene::base::extents<std::size_t, arene::base::dynamic_extent, 1, 1>>{
          arene::base::extents<std::size_t, arene::base::dynamic_extent, 1, 1>{1}
      }.is_exhaustive()
  );

  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::int32_t, 0, 0, 0>>::mapping<
          arene::base::extents<std::int32_t, arene::base::dynamic_extent, 1, 1>>{
          arene::base::extents<std::int32_t, arene::base::dynamic_extent, 1, 1>{1}
      }.is_exhaustive()
  );
}

template <std::size_t... Extents>
struct extent_values {
  template <typename IndexType>
  using type = arene::base::extents<IndexType, Extents...>;

  template <typename IndexType>
  static constexpr auto make_dextents() -> arene::base::dextents<IndexType, sizeof...(Extents)> {
    return arene::base::dextents<IndexType, sizeof...(Extents)>{Extents...};
  }
};

template <std::size_t... Offsets>
using offset_values = extent_values<Offsets...>;

template <typename Offsets, typename Extents, bool Exhaustive>
struct exhaustive_diagonal_values {
  using offsets = Offsets;
  using extents = Extents;
  static constexpr bool is_exhaustive = Exhaustive;
};

template <typename Offsets, typename Extents, bool Exhaustive>
constexpr bool exhaustive_diagonal_values<Offsets, Extents, Exhaustive>::is_exhaustive;

template <class OffsetsAndExtents>
class LayoutConstantDiagonalExhaustiveTest : public ::testing::Test {};

using exhaustive_diagonal_values_types = ::testing::Types<
    exhaustive_diagonal_values<offset_values<0>, extent_values<1>, false>,
    exhaustive_diagonal_values<offset_values<0>, extent_values<2>, false>,
    exhaustive_diagonal_values<offset_values<0>, extent_values<5>, false>,
    exhaustive_diagonal_values<offset_values<0, 0>, extent_values<2, 1>, true>,
    exhaustive_diagonal_values<offset_values<0, 0>, extent_values<1, 2>, true>,
    exhaustive_diagonal_values<offset_values<0, 3>, extent_values<4, 2>, true>,
    exhaustive_diagonal_values<offset_values<0, 3>, extent_values<2, 2>, false>,
    exhaustive_diagonal_values<offset_values<0, 3>, extent_values<10, 2>, true>,
    exhaustive_diagonal_values<offset_values<0, 3>, extent_values<2, 1>, false>,
    exhaustive_diagonal_values<offset_values<0, 3>, extent_values<1, 2>, false>,
    exhaustive_diagonal_values<offset_values<0, 0, 0>, extent_values<2, 1, 1>, true>,
    exhaustive_diagonal_values<offset_values<0, 0, 0>, extent_values<1, 1, 2>, true>,
    exhaustive_diagonal_values<offset_values<0, 0, 1>, extent_values<1, 1, 2>, false>,
    exhaustive_diagonal_values<offset_values<0, 2, 0>, extent_values<1, 1, 2>, false>,
    exhaustive_diagonal_values<offset_values<1, 1, 0>, extent_values<1, 1, 2>, true>,
    exhaustive_diagonal_values<offset_values<10, 10, 10>, extent_values<1, 1, 2>, true>,
    exhaustive_diagonal_values<offset_values<10, 10, 12>, extent_values<1, 1, 2>, false>,
    exhaustive_diagonal_values<offset_values<5, 0, 0>, extent_values<1, 2, 1>, false>,
    exhaustive_diagonal_values<offset_values<2, 0, 0>, extent_values<1, 2, 1>, false>,
    exhaustive_diagonal_values<offset_values<1, 0, 1>, extent_values<1, 2, 1>, true>,
    exhaustive_diagonal_values<offset_values<1, 0, 1>, extent_values<5, 3, 2>, true>,
    exhaustive_diagonal_values<offset_values<1, 0, 10>, extent_values<5, 3, 2>, false>,
    exhaustive_diagonal_values<offset_values<0, 0, 0>, extent_values<1, 5, 2>, true>,
    exhaustive_diagonal_values<offset_values<1, 3, 2>, extent_values<4, 5, 2>, true>,
    exhaustive_diagonal_values<offset_values<1, 3, 2>, extent_values<0, 5, 2>, true>,
    exhaustive_diagonal_values<offset_values<1, 3, 2>, extent_values<4, 0, 2>, true>,
    exhaustive_diagonal_values<offset_values<1, 3, 2>, extent_values<1, 1, 0>, true>,
    exhaustive_diagonal_values<offset_values<1, 3, 2>, extent_values<1, 1, 1>, false>>;

TYPED_TEST_SUITE(LayoutConstantDiagonalExhaustiveTest, exhaustive_diagonal_values_types, );

/// @test constant diagonal layouts are exhaustive if the largest dimension has extent 2 and there is a set of indices
/// such that (index+offset) for each dimension is equal
CONSTEXPR_TYPED_TEST(
    LayoutConstantDiagonalExhaustiveTest,
    LayoutsAreExhaustiveIfLargestDimensionHasExtent2AndIndexPlusOffsetCanBeEqualForAllDims
) {
  using static_offsets_type1 = typename TypeParam::offsets::template type<std::size_t>;
  using static_offsets_type2 = typename TypeParam::offsets::template type<std::int32_t>;
  using dynamic_offsets_type1 = arene::base::dextents<std::size_t, static_offsets_type1::rank()>;
  using dynamic_offsets_type2 = arene::base::dextents<std::int32_t, static_offsets_type1::rank()>;

  using static_extents_type1 = typename TypeParam::extents::template type<std::size_t>;
  using static_extents_type2 = typename TypeParam::extents::template type<std::int32_t>;
  using dynamic_extents_type1 = arene::base::dextents<std::size_t, static_extents_type1::rank()>;
  using dynamic_extents_type2 = arene::base::dextents<std::int32_t, static_extents_type1::rank()>;

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<static_offsets_type1>::template mapping<
           dynamic_extents_type1>{TypeParam::extents::template make_dextents<std::size_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<static_offsets_type1>::template mapping<
           dynamic_extents_type2>{TypeParam::extents::template make_dextents<std::int32_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<static_offsets_type2>::template mapping<
           dynamic_extents_type1>{TypeParam::extents::template make_dextents<std::size_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<static_offsets_type2>::template mapping<
           dynamic_extents_type2>{TypeParam::extents::template make_dextents<std::int32_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type1>::template mapping<
           dynamic_extents_type1>{
          TypeParam::extents::template make_dextents<std::size_t>(),
          TypeParam::offsets::template make_dextents<std::size_t>()
      }
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type1>::template mapping<
           dynamic_extents_type2>{
          TypeParam::extents::template make_dextents<std::int32_t>(),
          TypeParam::offsets::template make_dextents<std::size_t>()
      }
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type2>::template mapping<
           dynamic_extents_type1>{
          TypeParam::extents::template make_dextents<std::size_t>(),
          TypeParam::offsets::template make_dextents<std::int32_t>()
      }
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type2>::template mapping<
           dynamic_extents_type2>{
          TypeParam::extents::template make_dextents<std::int32_t>(),
          TypeParam::offsets::template make_dextents<std::int32_t>()
      }
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type1>::template mapping<
           static_extents_type1>{{}, TypeParam::offsets::template make_dextents<std::size_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type1>::template mapping<
           static_extents_type2>{{}, TypeParam::offsets::template make_dextents<std::size_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type2>::template mapping<
           static_extents_type1>{{}, TypeParam::offsets::template make_dextents<std::int32_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );

  CONSTEXPR_ASSERT_EQ(
      (typename arene::base::linalg::layout_constant_diagonal<dynamic_offsets_type2>::template mapping<
           static_extents_type2>{{}, TypeParam::offsets::template make_dextents<std::int32_t>()}
           .is_exhaustive()),
      TypeParam::is_exhaustive
  );
}

/// @test constant diagonal layouts with dynamic offsets are exhaustive if the largest dimension has extent 2 and there
/// is a set of indices such that (index+offset) for each dimension is equal
CONSTEXPR_TEST(
    LayoutConstantDiagonal,
    LayoutsWithDynamicOffsetsAreNotExhaustiveIfLargestDimensionHasExtent2AndIndexPlusOffsetCanBeEqualForAllDims
) {
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 1>>::mapping<
                             arene::base::extents<std::size_t, 2>>{{}, arene::base::dextents<std::size_t, 1>{0}}
                             .is_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 2>>::mapping<
                       arene::base::extents<std::size_t, 2, 1>>{{}, arene::base::dextents<std::size_t, 2>{0, 0}}
                       .is_exhaustive());
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 2>>::mapping<
                             arene::base::extents<std::size_t, 2, 1>>{{}, arene::base::dextents<std::size_t, 2>{0, 3}}
                             .is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                       arene::base::extents<std::size_t, 1, 1, 2>>{{}, arene::base::dextents<std::size_t, 3>{0, 0, 0}}
                       .is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                             arene::base::extents<std::size_t, 1, 1, 2>>{
      {},
      arene::base::dextents<std::size_t, 3>{0, 0, 1}
  }.is_exhaustive());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                             arene::base::extents<std::size_t, 1, 1, 2>>{
      {},
      arene::base::dextents<std::size_t, 3>{0, 2, 0}
  }.is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                       arene::base::extents<std::size_t, 1, 1, 2>>{{}, arene::base::dextents<std::size_t, 3>{1, 1, 0}}
                       .is_exhaustive());

  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                       arene::base::extents<std::size_t, 1, 1, 2>>{
      {},
      arene::base::dextents<std::size_t, 3>{10, 10, 10}
  }.is_exhaustive());
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                             arene::base::extents<std::size_t, 1, 1, 2>>{
      {},
      arene::base::dextents<std::size_t, 3>{10, 10, 12}
  }.is_exhaustive());
}

/// @test Constant diagonal layouts are not strided
CONSTEXPR_TEST(LayoutConstantDiagonal, NotStrided) {
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                             arene::base::extents<std::size_t, 2>>{}
                             .is_strided());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
          arene::base::extents<std::size_t, 10, 10>>{}
          .is_strided()
  );
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<
                             arene::base::extents<std::size_t, 0, 0>>::mapping<arene::base::extents<std::size_t, 0, 0>>{
  }
                             .is_strided());
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                             arene::base::dextents<std::size_t, 3>>{}
                             .is_strided());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
                             arene::base::extents<std::size_t>>{}
                             .is_strided());
}

/// @test Constant diagonal layouts are not always strided
CONSTEXPR_TEST(LayoutConstantDiagonal, NotAlwaysStrided) {
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                         arene::base::extents<std::size_t, 2>>::is_always_strided());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
          arene::base::extents<std::size_t, 10, 10>>::is_always_strided()
  );
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
          arene::base::extents<std::size_t, 0, 0>>::is_always_strided()
  );
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                         arene::base::dextents<std::size_t, 3>>::is_always_strided());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
                         arene::base::extents<std::size_t>>::is_always_strided());
}

/// @test Constant diagonal layouts are only always unique if all constant extents and unique
CONSTEXPR_TEST(LayoutConstantDiagonal, AlwaysUnique) {
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
                   arene::base::extents<std::size_t>>::is_always_unique());
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                         arene::base::extents<std::size_t, 2>>::is_always_unique());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                   arene::base::extents<std::size_t, 0>>::is_always_unique());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
          arene::base::extents<std::size_t, 10, 10>>::is_always_unique()
  );
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
                   arene::base::extents<std::size_t, 0, 10>>::is_always_unique());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
                   arene::base::extents<std::size_t, 5, 0>>::is_always_unique());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
                   arene::base::extents<std::size_t, 0, 0>>::is_always_unique());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
          arene::base::extents<std::size_t, 1, 1>>::is_always_unique()
  );
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
          arene::base::dextents<std::size_t, 2>>::is_always_unique()
  );
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                         arene::base::dextents<std::size_t, 3>>::is_always_unique());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                   arene::base::extents<std::size_t, 1, 1, 0>>::is_always_unique());
}

/// @test Constant diagonal layouts are only always exhaustive if offsets and extents are all static and is exhaustive
CONSTEXPR_TEST(LayoutConstantDiagonal, AlwaysExhaustive) {
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
                   arene::base::extents<std::size_t>>::is_always_exhaustive());
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                         arene::base::extents<std::size_t, 2>>::is_always_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
                   arene::base::extents<std::size_t, 0>>::is_always_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
                   arene::base::extents<std::size_t, 10, 10>>::is_always_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
                   arene::base::extents<std::size_t, 0, 10>>::is_always_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 10, 10>>::mapping<
                   arene::base::extents<std::size_t, 5, 0>>::is_always_exhaustive());
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
                   arene::base::extents<std::size_t, 0, 0>>::is_always_exhaustive());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
          arene::base::extents<std::size_t, 1, 1>>::is_always_exhaustive()
  );
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
                   arene::base::extents<std::size_t, 1, 2>>::is_always_exhaustive());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0, 0>>::mapping<
          arene::base::extents<std::size_t, 1, 1, 1>>::is_always_exhaustive()
  );
  CONSTEXPR_ASSERT(arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0, 0>>::mapping<
                   arene::base::extents<std::size_t, 1, 2, 1>>::is_always_exhaustive());
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 5, 0, 0>>::mapping<
          arene::base::extents<std::size_t, 1, 2, 1>>::is_always_exhaustive()
  );
  CONSTEXPR_ASSERT_FALSE(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
          arene::base::dextents<std::size_t, 2>>::is_always_exhaustive()
  );
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                         arene::base::dextents<std::size_t, 3>>::is_always_exhaustive());
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
                         arene::base::extents<std::size_t, 1, 1, 0>>::is_always_exhaustive());
}

/// @test The value is either offset 0 for on the diagonal, or offset 1 for off the diagonal
CONSTEXPR_TEST(LayoutConstantDiagonal, ValueSelection) {
  auto const zero_dim_mapping = arene::base::linalg::layout_constant_diagonal<
      arene::base::extents<std::size_t>>::mapping<arene::base::extents<std::size_t>>{};
  CONSTEXPR_ASSERT_EQ(zero_dim_mapping(), 0U);

  auto const one_dim_mapping = arene::base::linalg::layout_constant_diagonal<
      arene::base::extents<std::size_t, 0>>::mapping<arene::base::extents<std::size_t, 3>>{};
  CONSTEXPR_ASSERT_EQ(one_dim_mapping(0), 0U);
  CONSTEXPR_ASSERT_EQ(one_dim_mapping(1), 0U);
  CONSTEXPR_ASSERT_EQ(one_dim_mapping(2), 0U);

  auto const one_dim_dynamic_mapping =
      arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 1>>::mapping<
          arene::base::extents<std::size_t, 3>>{{}, arene::base::dextents<std::size_t, 1>{1}};
  CONSTEXPR_ASSERT_EQ(one_dim_dynamic_mapping(0), 0U);
  CONSTEXPR_ASSERT_EQ(one_dim_dynamic_mapping(1), 0U);
  CONSTEXPR_ASSERT_EQ(one_dim_dynamic_mapping(2), 0U);

  auto const two_dim_mapping = arene::base::linalg::layout_constant_diagonal<
      arene::base::extents<std::size_t, 0, 0>>::mapping<arene::base::extents<std::size_t, 3, 4>>{};
  for (unsigned i = 0; i < 3; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      CONSTEXPR_ASSERT_EQ(two_dim_mapping(i, j), (i == j) ? 0U : 1U);
    }
  }

  auto const two_dim_mapping_with_offset = arene::base::linalg::layout_constant_diagonal<
      arene::base::extents<std::size_t, 1, 2>>::mapping<arene::base::extents<std::size_t, 3, 4>>{};
  for (unsigned i = 0; i < 3; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      CONSTEXPR_ASSERT_EQ(two_dim_mapping_with_offset(i, j), ((i + 1) == (j + 2)) ? 0U : 1U);
    }
  }

  auto const three_dim_mapping = arene::base::linalg::layout_constant_diagonal<
      arene::base::extents<std::size_t, 0, 0, 0>>::mapping<arene::base::extents<std::size_t, 3, 4, 5>>{};
  for (unsigned i = 0; i < 3; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      for (unsigned k = 0; k < 5; ++k) {
        CONSTEXPR_ASSERT_EQ(three_dim_mapping(i, j, k), ((i == j) && (j == k)) ? 0U : 1U);
      }
    }
  }

  auto const three_dim_mapping_with_offset = arene::base::linalg::layout_constant_diagonal<
      arene::base::extents<std::size_t, 3, 2, 1>>::mapping<arene::base::extents<std::size_t, 3, 4, 5>>{};
  for (unsigned i = 0; i < 3; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      for (unsigned k = 0; k < 5; ++k) {
        CONSTEXPR_ASSERT_EQ(
            three_dim_mapping_with_offset(i, j, k),
            (((i + 3) == (j + 2)) && ((j + 2) == (k + 1))) ? 0U : 1U
        );
      }
    }
  }
}

/// @test Can get the extents for a mapping
CONSTEXPR_TEST(LayoutConstantDiagonal, CanGetExtents) {
  CONSTEXPR_ASSERT_EQ(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
          arene::base::extents<std::size_t>>{}
          .extents(),
      arene::base::extents<std::size_t>{}
  );

  auto const r1extents = arene::base::dextents<std::size_t, 1>{42};
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0>>::mapping<
           arene::base::dextents<std::size_t, 1>>{r1extents}
           .extents()),
      r1extents
  );

  auto const r2extents = arene::base::dextents<std::size_t, 2>{99, 123};
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0>>::mapping<
           arene::base::dextents<std::size_t, 2>>{r2extents}
           .extents()),
      r2extents
  );

  auto const r3extents = arene::base::dextents<std::size_t, 3>{99, 133, 2};
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t, 0, 0, 0>>::mapping<
           arene::base::dextents<std::size_t, 3>>{r3extents}
           .extents()),
      r3extents
  );
}

/// @test Can get the offsets for a mapping
CONSTEXPR_TEST(LayoutConstantDiagonal, CanGetOffsets) {
  CONSTEXPR_ASSERT_EQ(
      arene::base::linalg::layout_constant_diagonal<arene::base::extents<std::size_t>>::mapping<
          arene::base::extents<std::size_t>>{}
          .offsets(),
      arene::base::extents<std::size_t>{}
  );

  auto const r1offsets = arene::base::dextents<std::size_t, 1>{42};
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 1>>::mapping<
           arene::base::extents<std::size_t, 42>>{{}, r1offsets}
           .offsets()),
      r1offsets
  );

  auto const r2offsets = arene::base::dextents<std::size_t, 2>{99, 123};
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 2>>::mapping<
           arene::base::extents<std::size_t, 2, 4>>{{}, r2offsets}
           .offsets()),
      r2offsets
  );

  auto const r3offsets = arene::base::dextents<std::size_t, 3>{99, 133, 2};
  CONSTEXPR_ASSERT_EQ(
      (arene::base::linalg::layout_constant_diagonal<arene::base::dextents<std::size_t, 3>>::mapping<
           arene::base::extents<std::size_t, 3, 6, 9>>{{}, r3offsets}
           .offsets()),
      r3offsets
  );
}

}  // namespace
