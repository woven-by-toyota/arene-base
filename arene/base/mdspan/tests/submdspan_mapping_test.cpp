// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/mdspan/detail/do_submdspan_mapping.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"  // IWYU pragma: keep
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/reverse_as_tuple.hpp"

namespace {

/// @brief adapts extents for a given layout: identity for @c layout_left, reverse for @c layout_right
template <class Layout, class Extents>
struct adapt_extents {
  using type = Extents;
};

template <class Extents>
struct adapt_extents<arene::base::layout_right, Extents> {
  using type = arene::base::mdspan_detail::reverse_extents_t<Extents>;
};

template <class Layout, class Extents>
using adapt_extents_t = typename adapt_extents<Layout, Extents>::type;

/// @brief dispatches @c do_submdspan_mapping with slices in the appropriate order for @c Layout
template <class Layout>
struct adapted_submdspan_mapping_fn;

template <>
struct adapted_submdspan_mapping_fn<arene::base::layout_left> {
  template <class Mapping, class... Slices>
  constexpr auto operator()(Mapping const& mapping, Slices... slices) const {
    return arene::base::mdspan_detail::do_submdspan_mapping(mapping, slices...);
  }
};

template <>
struct adapted_submdspan_mapping_fn<arene::base::layout_right> {
  template <class Mapping, class... Slices>
  constexpr auto operator()(Mapping const& mapping, Slices... slices) const {
    return arene::base::apply(
        arene::base::bind_front(arene::base::mdspan_detail::do_submdspan_mapping, mapping),
        arene::base::tuple_detail::reverse_as_tuple(std::make_tuple(slices...))
    );
  }
};

template <class Layout>
constexpr auto adapted_submdspan_mapping = adapted_submdspan_mapping_fn<Layout>{};

/// @brief unit-stride @c extent_slice alias used across tests
using unit_slice = arene::base::extent_slice<std::int32_t, std::int32_t, std::integral_constant<std::int32_t, 1>>;

/// @brief constructs a default-initialized layout mapping with @p Extents adapted for @c Layout
template <class Layout, std::int32_t... Extents>
constexpr auto make_adapted_mapping() noexcept {
  using left_ext = arene::base::extents<std::int32_t, Extents...>;
  using ext = adapt_extents_t<Layout, left_ext>;
  using mapping_type = typename Layout::template mapping<ext>;
  return mapping_type{};
}

template <class T>
class SubmdspanMappingTest : public ::testing::Test {};

using submdspan_layout_types = ::testing::Types<arene::base::layout_left, arene::base::layout_right>;
TYPED_TEST_SUITE(SubmdspanMappingTest, submdspan_layout_types, );

// All tests are written in layout_left convention: the contiguous (stride-1)
// dimension is listed last in the extents, and slices are given in that same
// left-to-right order. The adaptation layer reverses extents and slices for
// layout_right.

/// @test @c submdspan_mapping with full_extent on non-boundary dims and a unit-stride extent_slice at the boundary
/// preserves the layout
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithFullExtentAndUnitStride) {
  using Layout = TypeParam;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, arene::base::full_extent, unit_slice{0, 3});

  using left_expected = arene::base::extents<std::int32_t, 5, 4, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<typename Layout::template mapping<expected_ext>, decltype(result.mapping)>::value,
      "must preserve layout"
  );

  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(3));
  CONSTEXPR_ASSERT_EQ(result.offset, 0U);
}

/// @test @c submdspan_mapping preserves the layout with a non-zero offset at the boundary
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithNonZeroOffsetAtBoundary) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, arene::base::full_extent, unit_slice{1, 2});

  using left_expected = arene::base::extents<std::int32_t, 5, 4, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<typename Layout::template mapping<expected_ext>, decltype(result.mapping)>::value,
      "must preserve layout"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(2));
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 0, 1)));
}

/// @test @c submdspan_mapping preserves the layout for a rank-1 unit-stride extent_slice with non-zero offset
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesForRank1UnitStrideExtentSlice) {
  using Layout = TypeParam;
  auto const mapping = make_adapted_mapping<Layout, 10>();
  auto const result = adapted_submdspan_mapping<Layout>(mapping, unit_slice{2, 5});

  static_assert(
      std::is_same<
          typename Layout::template mapping<arene::base::dextents<std::int32_t, 1>>,
          decltype(result.mapping)>::value,
      "must preserve layout for unit-stride extent_slice"
  );

  CONSTEXPR_ASSERT_EQ(result.mapping.extents().extent(0), std::int32_t{5});
  CONSTEXPR_ASSERT_EQ(result.offset, 2U);
}

/// @test @c submdspan_mapping preserves the layout when an index collapses a dimension beyond the boundary
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithIndexAndUnitStrideSlice) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, unit_slice{0, 4}, std::int32_t{1});

  using left_expected = arene::base::extents<std::int32_t, 5, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<typename Layout::template mapping<expected_ext>, decltype(result.mapping)>::value,
      "must preserve layout"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(4));
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 0, 1)));
}

/// @test @c submdspan_mapping preserves the layout when multiple indices collapse dimensions beyond the boundary
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithMultipleIndicesAndUnitStrideSlice) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3, 2>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3, 2>();
  auto const result = adapted_submdspan_mapping<Layout>(
      mapping,
      arene::base::full_extent,
      unit_slice{0, 4},
      std::int32_t{2},
      std::int32_t{1}
  );

  using left_expected = arene::base::extents<std::int32_t, 5, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<typename Layout::template mapping<expected_ext>, decltype(result.mapping)>::value,
      "must preserve layout"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(4));
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 0, 2, 1)));
}

/// @test @c submdspan_mapping preserves the layout when @c full_extent_t is at the boundary (not extent_slice)
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithFullExtentAtBoundary) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, arene::base::full_extent, std::int32_t{2});

  using left_expected = arene::base::extents<std::int32_t, 5, 4>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<typename Layout::template mapping<expected_ext>, decltype(result.mapping)>::value,
      "must preserve layout when full_extent_t is at the boundary"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext{});
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 0, 2)));
}

/// @test @c submdspan_mapping falls back to @c layout_stride when an integer is at the contiguous (stride-1) dimension
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, FallsBackWhenContiguousDimIsInteger) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, std::int32_t{1}, arene::base::full_extent, arene::base::full_extent);

  using left_expected = arene::base::extents<std::int32_t, 4, 3>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<arene::base::layout_stride::mapping<expected_ext>, decltype(result.mapping)>::value,
      "must fall back to layout_stride"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(1, 0, 0)));
}

/// @test @c submdspan_mapping falls back to @c layout_stride with a non-unit-stride extent_slice at the boundary
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, FallsBackWithNonUnitStrideAtBoundary) {
  using Layout = TypeParam;
  using slice_type = arene::base::extent_slice<std::int32_t, std::int32_t, std::integral_constant<std::int32_t, 2>>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 6>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, arene::base::full_extent, slice_type{0, 3});

  using left_expected = arene::base::extents<std::int32_t, 5, 4, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<arene::base::layout_stride::mapping<expected_ext>, decltype(result.mapping)>::value,
      "must fall back to layout_stride"
  );

  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(3));
  CONSTEXPR_ASSERT_EQ(result.offset, 0U);
}

/// @test @c submdspan_mapping falls back to @c layout_stride with a runtime-stride extent_slice even if stride value is
/// 1
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, FallsBackWithRuntimeStride) {
  using Layout = TypeParam;
  using slice_type = arene::base::extent_slice<int, int, int>;
  auto const mapping = make_adapted_mapping<Layout, 4, 3>();
  auto const result = adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, slice_type{0, 3, 1});

  using left_expected = arene::base::extents<std::int32_t, 4, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<arene::base::layout_stride::mapping<expected_ext>, decltype(result.mapping)>::value,
      "must fall back to layout_stride because stride type is not compile-time"
  );

  CONSTEXPR_ASSERT_EQ(result.mapping, mapping);
  CONSTEXPR_ASSERT_EQ(result.offset, 0U);
}

/// @test @c submdspan_mapping falls back to @c layout_stride when an index sits at the boundary position
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, FallsBackWithIndexAtBoundary) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, std::int32_t{1}, unit_slice{0, 3});

  using left_expected = arene::base::extents<std::int32_t, 5, arene::base::dynamic_extent>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<arene::base::layout_stride::mapping<expected_ext>, decltype(result.mapping)>::value,
      "must fall back to layout_stride when index is at boundary"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(3));
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 1, 0)));
}

/// @test @c submdspan_mapping falls back to @c layout_stride when an extent_slice is not at the boundary
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, FallsBackWithExtentSliceAtNonBoundary) {
  using Layout = TypeParam;
  using left_ext = arene::base::extents<std::int32_t, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, unit_slice{0, 5}, arene::base::full_extent, std::int32_t{1});

  using left_expected = arene::base::extents<std::int32_t, arene::base::dynamic_extent, 4>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<arene::base::layout_stride::mapping<expected_ext>, decltype(result.mapping)>::value,
      "must fall back to layout_stride when extent_slice is not at boundary"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(5));
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 0, 1)));
}

}  // namespace
