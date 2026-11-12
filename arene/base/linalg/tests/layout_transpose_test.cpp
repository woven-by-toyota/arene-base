// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/layout_transpose.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/tests/transpose_test_helpers.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::is_layout_mapping_v;
using arene::base::layout_right;
using arene::base::layout_stride;
using arene::base::linalg::layout_transpose;
using arene::base::linalg_testing::make_nested;
using arene::base::linalg_testing::make_transposed;
using arene::base::linalg_testing::transposed_mapping_for;
using arene::base::mdspan_detail::reverse_extents_t;

using transpose_nested_types = test::rank2_standard_layout_mappings;

template <typename NestedMapping>
class LayoutTransposeTest : public ::testing::Test {};

TYPED_TEST_SUITE(LayoutTransposeTest, transpose_nested_types, );

/// @test A @c layout_transpose has the expected typedefs
TYPED_TEST(LayoutTransposeTest, HasExpectedTypedefs) {
  using transposed_mapping = transposed_mapping_for<TypeParam>;

  ::testing::StaticAssertTypeEq<
      typename transposed_mapping::extents_type,
      reverse_extents_t<typename TypeParam::extents_type>>();
  ::testing::StaticAssertTypeEq<typename transposed_mapping::index_type, typename TypeParam::extents_type::index_type>(
  );
  ::testing::StaticAssertTypeEq<typename transposed_mapping::size_type, typename TypeParam::extents_type::size_type>();
  ::testing::StaticAssertTypeEq<typename transposed_mapping::rank_type, typename TypeParam::extents_type::rank_type>();
  ::testing::
      StaticAssertTypeEq<typename transposed_mapping::layout_type, layout_transpose<typename TypeParam::layout_type>>();
  // nested_layout_type is a member of the layout policy, not the mapping.
  ::testing::StaticAssertTypeEq<
      typename layout_transpose<typename TypeParam::layout_type>::nested_layout_type,
      typename TypeParam::layout_type>();
  ::testing::StaticAssertTypeEq<typename transposed_mapping::nested_mapping_type, TypeParam>();
}

/// @test The transposed extents are the reverse of the nested mapping's extents, by both type and value
CONSTEXPR_TYPED_TEST(LayoutTransposeTest, ExtentsAreTransposed) {
  constexpr auto nested = make_nested<TypeParam>();
  constexpr auto layout = make_transposed<TypeParam>();
  CONSTEXPR_ASSERT_EQ(layout.extents().extent(0U), nested.extents().extent(1U));
  CONSTEXPR_ASSERT_EQ(layout.extents().extent(1U), nested.extents().extent(0U));
}

/// @test Indexing a transposed mapping reverses the indices passed to the nested mapping
CONSTEXPR_TYPED_TEST(LayoutTransposeTest, AccessReversesIndices) {
  using index_type = typename TypeParam::index_type;
  constexpr auto nested = make_nested<TypeParam>();
  constexpr auto layout = make_transposed<TypeParam>();

  // the nested layouts are all strided with a zero origin offset, so the flat offset of nested
  // index (j, i) is j * nested.stride(0) + i * nested.stride(1). Comparing against this (rather than nested(j, i),
  // which is exactly how layout(i, j) is implemented) verifies the index swap produces the correct offset.
  for (index_type i{}; i < layout.extents().extent(0U); ++i) {
    for (index_type j{}; j < layout.extents().extent(1U); ++j) {
      auto const expected = static_cast<index_type>(j * nested.stride(0U) + i * nested.stride(1U));
      CONSTEXPR_ASSERT_EQ(layout(i, j), expected);
    }
  }
}

/// @test A transposed mapping swaps the two strides of the wrapped mapping
CONSTEXPR_TYPED_TEST(LayoutTransposeTest, StridesAreSwapped) {
  constexpr auto nested = make_nested<TypeParam>();
  constexpr auto layout = make_transposed<TypeParam>();

  CONSTEXPR_ASSERT_EQ(layout.stride(0U), nested.stride(1U));
  CONSTEXPR_ASSERT_EQ(layout.stride(1U), nested.stride(0U));
}

/// @test `required_span_size`, `nested_mapping`, and the property queries delegate to the wrapped mapping
CONSTEXPR_TYPED_TEST(LayoutTransposeTest, DelegatesToNestedMapping) {
  using transposed_mapping_type = transposed_mapping_for<TypeParam>;
  constexpr auto nested = make_nested<TypeParam>();
  constexpr auto layout = make_transposed<TypeParam>();

  CONSTEXPR_ASSERT_EQ(layout.required_span_size(), nested.required_span_size());
  CONSTEXPR_ASSERT(layout.nested_mapping() == nested);

  CONSTEXPR_ASSERT_EQ(transposed_mapping_type::is_always_unique(), TypeParam::is_always_unique());
  CONSTEXPR_ASSERT_EQ(transposed_mapping_type::is_always_exhaustive(), TypeParam::is_always_exhaustive());
  CONSTEXPR_ASSERT_EQ(transposed_mapping_type::is_always_strided(), TypeParam::is_always_strided());
  CONSTEXPR_ASSERT_EQ(layout.is_unique(), nested.is_unique());
  CONSTEXPR_ASSERT_EQ(layout.is_exhaustive(), nested.is_exhaustive());
  CONSTEXPR_ASSERT_EQ(layout.is_strided(), nested.is_strided());
}

/// @test Two transposed mappings compare equal iff their wrapped nested mappings do
CONSTEXPR_TYPED_TEST(LayoutTransposeTest, EqualityComparesNestedMappings) {
  constexpr auto same_a = make_transposed<TypeParam>();
  constexpr auto same_b = make_transposed<TypeParam>();
  CONSTEXPR_ASSERT(same_a == same_b);
  CONSTEXPR_ASSERT(!(same_a != same_b));
}

/// @test A transposed mapping satisfies the layout mapping requirements
TYPED_TEST(LayoutTransposeTest, SatisfiesLayoutMappingRequirements) {
  STATIC_ASSERT_TRUE(is_layout_mapping_v<transposed_mapping_for<TypeParam>>);
  ::testing::StaticAssertTypeEq<
      typename transposed_mapping_for<TypeParam>::layout_type,
      layout_transpose<typename TypeParam::layout_type>>();
}

/// @test Wrapping a `layout_stride` preserves its (swapped) strides
CONSTEXPR_TEST(LayoutTransposeTest, PreservesNestedStrideLayout) {
  using nested_extents = extents<std::uint32_t, 3, dynamic_extent>;
  using transposed_extents = extents<std::uint32_t, dynamic_extent, 3>;
  using stride_mapping = layout_stride::mapping<nested_extents>;
  constexpr std::uint32_t cols{5U};
  constexpr std::uint32_t row_stride{10U};
  constexpr std::uint32_t col_stride{1U};
  // Non-default strides for extents (3, 5): a strided, non-exhaustive layout.
  constexpr stride_mapping nested{nested_extents{cols}, arene::base::array<std::uint32_t, 2U>{row_stride, col_stride}};
  constexpr layout_transpose<layout_stride>::mapping<transposed_extents> layout{nested};

  CONSTEXPR_ASSERT_EQ(layout.stride(0U), col_stride);
  CONSTEXPR_ASSERT_EQ(layout.stride(1U), row_stride);
  CONSTEXPR_ASSERT_EQ(layout.is_exhaustive(), nested.is_exhaustive());
  CONSTEXPR_ASSERT_EQ(layout.required_span_size(), nested.required_span_size());
}

/// @test A transposed mapping over extents with a zero dimension reverses the extents and reports a zero span size
CONSTEXPR_TEST(LayoutTransposeTest, ZeroExtent) {
  constexpr std::uint32_t zero{0U};
  constexpr std::uint32_t four{4U};
  using nested_extents = extents<std::uint32_t, 0, 4>;
  using transposed_extents = extents<std::uint32_t, 4, 0>;
  constexpr layout_right::mapping<nested_extents> nested{};
  constexpr layout_transpose<layout_right>::mapping<transposed_extents> layout{nested};

  CONSTEXPR_ASSERT_EQ(layout.extents().extent(0U), four);
  CONSTEXPR_ASSERT_EQ(layout.extents().extent(1U), zero);
  // required_span_size is 0 because one extent is 0.
  CONSTEXPR_ASSERT_EQ(layout.required_span_size(), zero);
  CONSTEXPR_ASSERT_EQ(layout.stride(0U), nested.stride(1U));
  CONSTEXPR_ASSERT_EQ(layout.stride(1U), nested.stride(0U));
}

/// @test `stride` succeeds on a nested mapping that is not always strided but is strided at runtime
///
/// This is the only combination for which the runtime strided check in `stride()` is meaningful: `is_always_strided()`
/// is false (so the check is not statically elided) yet `is_strided()` is true (so the precondition passes). The nested
/// type matches the one in the death test's non-strided case, so together they cover both sides of that check.
CONSTEXPR_TEST(LayoutTransposeTest, StrideSucceedsWhenSometimesStridedMappingIsStrided) {
  // is_always_strided() == false, but the runtime value passed below makes is_strided() == true.
  using nested_layout = test::user_defined_layout</*Strided=*/false, /*Exhaustive=*/true, /*Unique=*/true>;
  using nested_extents = extents<std::uint32_t, 3, 5>;
  using transposed_extents = extents<std::uint32_t, 5, 3>;
  constexpr nested_layout::mapping<nested_extents> nested{/*is_strided=*/true};
  constexpr layout_transpose<nested_layout>::mapping<transposed_extents> layout{nested};

  CONSTEXPR_ASSERT(layout.is_strided());
  CONSTEXPR_ASSERT_EQ(layout.stride(0U), nested.stride(1U));
  CONSTEXPR_ASSERT_EQ(layout.stride(1U), nested.stride(0U));
}

/// @test The `is_always_*` queries delegate to the nested layout, including unusual property combinations that the
/// standard layouts never produce
TEST(LayoutTransposeTest, DelegatesAlwaysProperties) {
  // user_defined_layout<Strided, Exhaustive, Unique> lets us pick a combination distinct from any standard layout.
  using nested_layout = test::user_defined_layout</*Strided=*/false, /*Exhaustive=*/false, /*Unique=*/true>;
  using transposed_mapping_type = layout_transpose<nested_layout>::mapping<extents<std::uint32_t, 5, 3>>;
  using nested_mapping_type = transposed_mapping_type::nested_mapping_type;

  STATIC_ASSERT_EQ(transposed_mapping_type::is_always_unique(), nested_mapping_type::is_always_unique());
  STATIC_ASSERT_EQ(transposed_mapping_type::is_always_exhaustive(), nested_mapping_type::is_always_exhaustive());
  STATIC_ASSERT_EQ(transposed_mapping_type::is_always_strided(), nested_mapping_type::is_always_strided());
  STATIC_ASSERT_TRUE(transposed_mapping_type::is_always_unique());
  STATIC_ASSERT_FALSE(transposed_mapping_type::is_always_exhaustive());
  STATIC_ASSERT_FALSE(transposed_mapping_type::is_always_strided());
}

}  // namespace
