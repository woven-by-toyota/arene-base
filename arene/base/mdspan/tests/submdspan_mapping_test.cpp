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
#include "arene/base/stdlib_choice/tuple.hpp"           // IWYU pragma: keep
#include "arene/base/testing/default_distribution.hpp"  // IWYU pragma: keep
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/reverse_as_tuple.hpp"
#include "arene/base/type_list/cartesian_product.hpp"

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
template <class IndexType>
using unit_slice = arene::base::extent_slice<IndexType, IndexType, std::integral_constant<IndexType, 1>>;

/// @brief constructs a default-initialized layout mapping with @p Extents adapted for @c Layout
template <class Layout, class IndexType, std::size_t... Extents>
constexpr auto make_adapted_mapping() noexcept {
  using left_ext = arene::base::extents<IndexType, Extents...>;
  using ext = adapt_extents_t<Layout, left_ext>;
  using mapping_type = typename Layout::template mapping<ext>;
  return mapping_type{};
}

/// @brief a layout policy paired with the index type of the mappings under test
template <class Layout, class IndexType>
struct layout_index_case {
  using layout_type = Layout;
  using index_type = IndexType;
};

template <class T>
class SubmdspanMappingTest : public ::testing::Test {};

using submdspan_layout_types = ::testing::Types<arene::base::layout_left, arene::base::layout_right>;
using submdspan_index_types = ::testing::Types<std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t>;
using submdspan_mapping_test_cases =
    arene::base::type_lists::cartesian_product_as_t<layout_index_case, submdspan_layout_types, submdspan_index_types>;
TYPED_TEST_SUITE(SubmdspanMappingTest, submdspan_mapping_test_cases, );

// All tests are written in layout_left convention: the contiguous (stride-1)
// dimension is listed last in the extents, and slices are given in that same
// left-to-right order. The adaptation layer reverses extents and slices for
// layout_right.

/// @test @c submdspan_mapping with full_extent on non-boundary dims and a unit-stride extent_slice at the boundary
/// preserves the layout
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithFullExtentAndUnitStride) {
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result = adapted_submdspan_mapping<Layout>(
      mapping,
      arene::base::full_extent,
      arene::base::full_extent,
      unit_slice<index_type>{0, 3}
  );

  using left_expected = arene::base::extents<index_type, 5, 4, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result = adapted_submdspan_mapping<Layout>(
      mapping,
      arene::base::full_extent,
      arene::base::full_extent,
      unit_slice<index_type>{1, 2}
  );

  using left_expected = arene::base::extents<index_type, 5, 4, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  auto const mapping = make_adapted_mapping<Layout, index_type, 10>();
  auto const result = adapted_submdspan_mapping<Layout>(mapping, unit_slice<index_type>{2, 5});

  static_assert(
      std::is_same<typename Layout::template mapping<arene::base::dextents<index_type, 1>>, decltype(result.mapping)>::
          value,
      "must preserve layout for unit-stride extent_slice"
  );

  CONSTEXPR_ASSERT_EQ(result.mapping.extents().extent(0), index_type{5});
  CONSTEXPR_ASSERT_EQ(result.offset, 2U);
}

/// @test @c submdspan_mapping preserves the layout when an index collapses a dimension beyond the boundary
CONSTEXPR_TYPED_TEST(SubmdspanMappingTest, PreservesWithIndexAndUnitStrideSlice) {
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, unit_slice<index_type>{0, 4}, index_type{1});

  using left_expected = arene::base::extents<index_type, 5, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3, 2>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3, 2>();
  auto const result = adapted_submdspan_mapping<Layout>(
      mapping,
      arene::base::full_extent,
      unit_slice<index_type>{0, 4},
      index_type{2},
      index_type{1}
  );

  using left_expected = arene::base::extents<index_type, 5, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, arene::base::full_extent, index_type{2});

  using left_expected = arene::base::extents<index_type, 5, 4>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, index_type{1}, arene::base::full_extent, arene::base::full_extent);

  using left_expected = arene::base::extents<index_type, 4, 3>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using slice_type = arene::base::extent_slice<index_type, index_type, std::integral_constant<index_type, 2>>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 6>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, arene::base::full_extent, slice_type{0, 3});

  using left_expected = arene::base::extents<index_type, 5, 4, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using slice_type = arene::base::extent_slice<index_type, index_type, index_type>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 4, 3>();
  auto const result = adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, slice_type{0, 3, 1});

  using left_expected = arene::base::extents<index_type, 4, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, arene::base::full_extent, index_type{1}, unit_slice<index_type>{0, 3});

  using left_expected = arene::base::extents<index_type, 5, arene::base::dynamic_extent>;
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
  using Layout = typename TypeParam::layout_type;
  using index_type = typename TypeParam::index_type;
  using left_ext = arene::base::extents<index_type, 5, 4, 3>;
  auto const mapping = make_adapted_mapping<Layout, index_type, 5, 4, 3>();
  auto const result =
      adapted_submdspan_mapping<Layout>(mapping, unit_slice<index_type>{0, 5}, arene::base::full_extent, index_type{1});

  using left_expected = arene::base::extents<index_type, arene::base::dynamic_extent, 4>;
  using expected_ext = adapt_extents_t<Layout, left_expected>;
  static_assert(
      std::is_same<arene::base::layout_stride::mapping<expected_ext>, decltype(result.mapping)>::value,
      "must fall back to layout_stride when extent_slice is not at boundary"
  );

  arene::base::layout_left::mapping<left_ext> const left_mapping{};
  CONSTEXPR_ASSERT_EQ(result.mapping.extents(), expected_ext(5));
  CONSTEXPR_ASSERT_EQ(result.offset, static_cast<std::size_t>(left_mapping(0, 0, 1)));
}

/// @brief one dimension of a runtime slicing case: an extent and a unit-stride slice that stays within it
/// @tparam IndexType index type of the mapping under test
template <class IndexType>
struct sliced_dimension {
  /// @brief extent of the source mapping in this dimension
  IndexType extent;
  /// @brief first index of the slice
  IndexType offset;
  /// @brief number of indices in the slice, with <c> offset + length <= extent </c>
  IndexType length;

  /// @brief the slice as a canonical unit-stride @c extent_slice
  constexpr auto slice() const noexcept -> unit_slice<IndexType> { return {offset, length}; }
};

/// @brief draws a @c sliced_dimension with an extent in [2, 8) and an in-bounds slice, empty slices included
/// @tparam IndexType index type of the mapping under test
///
/// The extents are kept small so the mapped index space is representable for every index type under test.
template <class IndexType>
class sliced_dimension_distribution {
 public:
  /// @brief The value type produced by this distribution
  using result_type = sliced_dimension<IndexType>;

  /// @brief draw a @c sliced_dimension
  /// @tparam PRNG A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return a dimension whose slice lies within its extent
  template <class PRNG>
  constexpr auto operator()(PRNG& prng) const noexcept(noexcept(prng())) -> result_type {
    auto const extent = 2 + draw_below(prng, 6);
    auto const offset = draw_below(prng, extent);
    auto const length = draw_below(prng, extent - offset + 1);
    return {static_cast<IndexType>(extent), static_cast<IndexType>(offset), static_cast<IndexType>(length)};
  }

 private:
  /// @brief draw an integer in [0, bound)
  template <class PRNG>
  static constexpr auto draw_below(PRNG& prng, int bound) noexcept(noexcept(prng())) -> int {
    return static_cast<int>(prng() % static_cast<typename PRNG::result_type>(bound));
  }
};

}  // namespace

// Generate the sliced_dimension parameters of property tests with the bounded distribution.
//
// A qualified declarator-id is used so the enclosing arene::base::testing namespace need not be reopened.
#ifndef ARENE_DOC_GENERATION_RUNNING
template <class IndexType>
constexpr auto arene::base::testing::default_distribution<sliced_dimension<IndexType>> =
    sliced_dimension_distribution<IndexType>{};
#endif

namespace {

/// @test Given a rank-2 mapping whose extents are generated at runtime, when it is sliced in both dimensions with
/// unit-stride @c extent_slice values whose offsets and lengths are also generated at runtime, then the result falls
/// back to @c layout_stride, reports the sliced extents, keeps the source strides, and offsets to the slice origin
///
/// Runtime-generated inputs keep the slicing arithmetic out of constant folding so the code emitted for the target is
/// what gets exercised; see https://github.com/llvm/llvm-project/issues/205291 for the miscompilation this guards.
ARENE_TYPED_PROPERTY(
    SubmdspanMappingTest,
    TwoRuntimeExtentSlicesFallBackAndKeepSourceStrides,
    (sliced_dimension<typename TypeParam::index_type> dim0, sliced_dimension<typename TypeParam::index_type> dim1)
) {
  using Layout = typename TypeParam::layout_type;
  using extents_type = arene::base::dextents<typename TypeParam::index_type, 2>;

  typename Layout::template mapping<extents_type> const mapping{extents_type{dim0.extent, dim1.extent}};
  auto const result = arene::base::mdspan_detail::do_submdspan_mapping(mapping, dim0.slice(), dim1.slice());

  static_assert(
      std::is_same<arene::base::layout_stride::mapping<extents_type>, decltype(result.mapping)>::value,
      "two extent slices must fall back to layout_stride"
  );

  ARENE_PROP_ASSERT(result.mapping.extents() == extents_type{dim0.length, dim1.length});
  ARENE_PROP_ASSERT(result.mapping.stride(0) == mapping.stride(0));
  ARENE_PROP_ASSERT(result.mapping.stride(1) == mapping.stride(1));
  ARENE_PROP_ASSERT(result.offset == static_cast<std::size_t>(mapping(dim0.offset, dim1.offset)));
}

}  // namespace
