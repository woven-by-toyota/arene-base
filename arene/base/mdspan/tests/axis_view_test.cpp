// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/axis_view.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/submdspan.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/ranges/is_borrowed_range.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// IWYU pragma: no_include "arene/base/stdlib_choice/tuple.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/make_tuple.hpp"

namespace {

using arene::base::apply;
using arene::base::axis_view;
using arene::base::bind_front;
using arene::base::full_extent;
using arene::base::submdspan;
using arene::base::linalg::equal_elements;
using arene::base::testing::concat_slices;
using arene::base::testing::equal_address;
using arene::base::testing::make_full_extents;
using arene::base::testing::make_test_mdspan;

/// @brief Representative source mdspan types spanning ranks 0-3 with static, dynamic, and mixed extents
using source_span_types = ::testing::Types<
    arene::base::mdspan<int const, arene::base::extents<std::size_t, 4>>,             // rank 1 static
    arene::base::mdspan<double const, arene::base::dextents<std::size_t, 1>>,         // rank 1 dynamic
    arene::base::mdspan<float const, arene::base::extents<std::size_t, 3, 4>>,        // rank 2 static
    arene::base::mdspan<std::uint32_t const, arene::base::dextents<std::size_t, 2>>,  // rank 2 dynamic
    arene::base::mdspan<int const, arene::base::extents<std::size_t, 2, arene::base::dynamic_extent>>,  // rank 2 mixed
    arene::base::mdspan<std::int16_t const, arene::base::extents<std::size_t, 2, 3, 4>>>;               // rank 3 static

template <class Mdspan>
class AxisViewTypedTest : public ::testing::Test {};

TYPED_TEST_SUITE(AxisViewTypedTest, source_span_types, );

/// @test axis_view collapsing the first axis is invocable for a source of any rank >= 1
TYPED_TEST(AxisViewTypedTest, IsInvocableForFirstAxis) {
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(axis_view<0>), TypeParam>));
}

/// @test axis_view is invocable with a canonicalizable index
TYPED_TEST(AxisViewTypedTest, IsInvocableWithIndex) {
  // "Index" types which are not actually canonicalizable indices (or are statically out of bounds) will hit a
  // static_assert and make the program ill-formed instead of being non-invocable, so we can't test them here.
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(axis_view<0>), TypeParam, std::size_t>));
  STATIC_ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(axis_view<0>), TypeParam, std::integral_constant<std::size_t, 0>>)
  );
}

/// @test the view iterator is an input iterator (each slice is a prvalue submdspan)
TYPED_TEST(AxisViewTypedTest, IteratorIsInputCategory) {
  using iterator_type = decltype(axis_view<0>(std::declval<TypeParam>()).begin());

  ::testing::StaticAssertTypeEq<typename iterator_type::iterator_category, std::input_iterator_tag>();
}

/// @test the view is a borrowed range, so its iterators stay valid after the range object is gone
TYPED_TEST(AxisViewTypedTest, ResultIsBorrowedRange) {
  using range_type = decltype(axis_view<0>(std::declval<TypeParam>()));

  STATIC_ASSERT_TRUE(arene::base::ranges::is_borrowed_range_v<range_type>);
}

/// @test axis_view rejects non-mdspan argument types
TEST(AxisViewTest, RejectsNonMdspanTypes) {
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(axis_view<0>), int>));
}

/// @test axis_view rejects an mdspan whose layout mapping is not sliceable
TEST(AxisViewTest, RejectsNonSliceableLayout) {
  using extents_type = arene::base::extents<std::size_t, 3, 4>;
  using non_sliceable_layout = test::basic_layout_mapping<extents_type>::layout_type;
  using non_sliceable_mdspan = arene::base::mdspan<int const, extents_type, non_sliceable_layout>;

  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(axis_view<0>), non_sliceable_mdspan>));
}

/// @test each slice of @c axis_view<0> aliases @c submdspan(src,fixed,full_extent...), for any source of rank >= 1
CONSTEXPR_TYPED_TEST(AxisViewTypedTest, SlicesAliasSubmdspanAlongAxis0) {
  auto const src = make_test_mdspan<TypeParam>();
  auto const submdspan_of_src = bind_front(submdspan, src);

  auto fixed = typename TypeParam::index_type{0};
  for (auto const slice : axis_view<0>(src)) {
    auto const slices = concat_slices(fixed, make_full_extents<TypeParam::rank() - 1>());
    CONSTEXPR_ASSERT(equal_elements(slice, apply(submdspan_of_src, slices), equal_address{}));
    CONSTEXPR_ASSERT(equal_elements(axis_view<0>(src, fixed), apply(submdspan_of_src, slices), equal_address{}));
    ++fixed;
  }
  CONSTEXPR_ASSERT(fixed == src.extent(0));
}

/// @test each slice of @c axis_view<1> aliases @c submdspan(src,full_extent,fixed,full_extent...), for rank >= 2
CONDITIONAL_CONSTEXPR_TYPED_TEST(AxisViewTypedTest, SlicesAliasSubmdspanAlongAxis1, TypeParam::rank() >= 2) {
  auto const src = make_test_mdspan<TypeParam>();
  auto const submdspan_of_src = bind_front(submdspan, src);

  auto fixed = typename TypeParam::index_type{0};
  for (auto const slice : axis_view<1>(src)) {
    auto const slices = concat_slices(full_extent, fixed, make_full_extents<TypeParam::rank() - 2>());
    CONSTEXPR_ASSERT(equal_elements(slice, apply(submdspan_of_src, slices), equal_address{}));
    CONSTEXPR_ASSERT(equal_elements(axis_view<1>(src, fixed), apply(submdspan_of_src, slices), equal_address{}));
    ++fixed;
  }
  CONSTEXPR_ASSERT(fixed == src.extent(1));
}

/// @test each slice of @c axis_view<2> aliases @c submdspan(src,full_extent,full_extent,fixed), for rank >= 3
CONDITIONAL_CONSTEXPR_TYPED_TEST(AxisViewTypedTest, SlicesAliasSubmdspanAlongAxis2, TypeParam::rank() >= 3) {
  auto const src = make_test_mdspan<TypeParam>();
  auto const submdspan_of_src = bind_front(submdspan, src);

  auto fixed = typename TypeParam::index_type{0};
  for (auto const slice : axis_view<2>(src)) {
    auto const slices = concat_slices(full_extent, full_extent, fixed, make_full_extents<TypeParam::rank() - 3>());
    CONSTEXPR_ASSERT(equal_elements(slice, apply(submdspan_of_src, slices), equal_address{}));
    CONSTEXPR_ASSERT(equal_elements(axis_view<2>(src, fixed), apply(submdspan_of_src, slices), equal_address{}));
    ++fixed;
  }
  CONSTEXPR_ASSERT(fixed == src.extent(2));
}

}  // namespace
