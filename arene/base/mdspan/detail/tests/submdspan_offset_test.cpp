// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/submdspan_offset.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::extent_slice;
using arene::base::extents;
using arene::base::full_extent;
using arene::base::layout_left;
using arene::base::layout_right;
using arene::base::mdspan_detail::submdspan_offset;

using layout_types = ::testing::Types<layout_left, layout_right>;

template <class T>
class SubmdspanOffsetTest : public ::testing::Test {
 public:
  using default_mapping = typename T::template mapping<extents<std::size_t, 30, 20>>;
};

TYPED_TEST_SUITE(SubmdspanOffsetTest, layout_types, );

/// @test @c submdspan_offset computes the correct offset with integral slices
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, IntegralSlices) {
  auto const mapping = typename SubmdspanOffsetTest<TypeParam>::default_mapping{};

  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{0}, std::size_t{0}) == mapping(0, 0));
  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{1}, std::size_t{0}) == mapping(1, 0));
  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{0}, std::size_t{1}) == mapping(0, 1));
}

/// @test @c submdspan_offset computes the correct offset with integral-constant slices
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, IntegralConstantSlices) {
  auto const mapping = typename SubmdspanOffsetTest<TypeParam>::default_mapping{};

  CONSTEXPR_ASSERT(
      submdspan_offset(mapping, std::integral_constant<std::size_t, 0>{}, std::size_t{0}) == mapping(0, 0)
  );
  CONSTEXPR_ASSERT(
      submdspan_offset(mapping, std::integral_constant<std::size_t, 1>{}, std::size_t{0}) == mapping(1, 0)
  );
  CONSTEXPR_ASSERT(
      submdspan_offset(mapping, std::integral_constant<std::size_t, 0>{}, std::integral_constant<std::size_t, 1>{}) ==
      mapping(0, 1)
  );
}

/// @test @c submdspan_offset computes the correct offset with an extent_slice
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, ExtentSlice) {
  auto const mapping = typename SubmdspanOffsetTest<TypeParam>::default_mapping{};

  auto slice = extent_slice<std::size_t>{std::size_t{1}, std::size_t{10}};
  CONSTEXPR_ASSERT(submdspan_offset(mapping, slice, full_extent) == mapping(1, 0));

  auto ic_slice = extent_slice<std::integral_constant<std::size_t, 1>, std::integral_constant<std::size_t, 10>>{};
  CONSTEXPR_ASSERT(submdspan_offset(mapping, ic_slice, full_extent) == mapping(1, 0));
}

/// @test @c submdspan_offset with full_extent slices
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, FullExtentSlices) {
  auto const mapping = typename SubmdspanOffsetTest<TypeParam>::default_mapping{};

  CONSTEXPR_ASSERT(submdspan_offset(mapping, full_extent, full_extent) == mapping(0, 0));
}

/// @test @c submdspan_offset works with a 1D mapping and with dynamic extents
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, OtherExtentsTypes) {
  auto mapping_1d = typename TypeParam::template mapping<extents<std::size_t, 10>>{};
  CONSTEXPR_ASSERT(submdspan_offset(mapping_1d, std::size_t{5}) == mapping_1d(5));

  using dyn_extents = extents<std::size_t, arene::base::dynamic_extent, arene::base::dynamic_extent>;
  auto mapping_dyn = typename TypeParam::template mapping<dyn_extents>{dyn_extents{std::size_t{3}, std::size_t{4}}};
  CONSTEXPR_ASSERT(submdspan_offset(mapping_dyn, std::size_t{2}, std::size_t{1}) == mapping_dyn(2, 1));
}

/// @test @c submdspan_offset with full_extent and mixed slices
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, FullExtent) {
  auto const mapping = typename SubmdspanOffsetTest<TypeParam>::default_mapping{};

  CONSTEXPR_ASSERT(submdspan_offset(mapping, full_extent, full_extent) == mapping(0, 0));
  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{0}, full_extent) == mapping(0, 0));
  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{10}, full_extent) == mapping(10, 0));
  CONSTEXPR_ASSERT(submdspan_offset(mapping, full_extent, std::size_t{0}) == mapping(0, 0));
  CONSTEXPR_ASSERT(submdspan_offset(mapping, full_extent, std::size_t{10}) == mapping(0, 10));
}

/// @test @c submdspan_offset returns required_span_size when an integral slice equals the extent
CONSTEXPR_TYPED_TEST(SubmdspanOffsetTest, SliceAtExtentBoundary) {
  auto const mapping = typename SubmdspanOffsetTest<TypeParam>::default_mapping{};

  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{30}, std::size_t{0}) == mapping.required_span_size());
  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{0}, std::size_t{20}) == mapping.required_span_size());
  CONSTEXPR_ASSERT(submdspan_offset(mapping, std::size_t{30}, std::size_t{20}) == mapping.required_span_size());
}

}  // namespace
