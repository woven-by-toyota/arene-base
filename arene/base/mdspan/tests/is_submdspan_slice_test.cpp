// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/is_submdspan_slice.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/testing/gtest.hpp"

struct int_pair_like {
  template <std::size_t>
  auto get() const -> int {
    return 0;
  }
};

template <>
class std::tuple_size<int_pair_like> : public std::integral_constant<std::size_t, 2> {};

template <std::size_t I>
class std::tuple_element<I, int_pair_like> {
 public:
  using type = int;
};

namespace {
using ::arene::base::extent_slice;
using ::arene::base::full_extent_t;
using ::arene::base::is_submdspan_slice_v;
using ::arene::base::range_slice;

/// @test for all the common slice types, @c is_submdspan_slice_v is @c true
TEST(IsSubmdspanSlice, StandardSliceTypes) {
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, full_extent_t>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, full_extent_t const>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, full_extent_t const&>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, full_extent_t&&>);

  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, int>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, int&>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, int const&>);

  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::size_t>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::size_t&>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::size_t const&>);

  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, extent_slice<int>>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, range_slice<int>>);

  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, extent_slice<std::integral_constant<int, 1>>>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, range_slice<std::integral_constant<int, 1>>>);

  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::pair<int, int>>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::tuple<int, int>>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, arene::base::array<int, 2>>);

  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::pair<int, int>&>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, std::tuple<int, int>&>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, arene::base::array<int, 2>&>);
}

/// @test @c is_submdspan_slice_v is @c true for a type that is implicitly convertible to the index type
TEST(IsSubmdspanSlice, ConvertibleToIndex) {  //
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, float>);

  struct my_int {
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator int() const { return 0; }
  };
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, my_int>);
}

/// @test @c is_submdspan_slice_v is @c true for a type derived from full_extent_t
TEST(IsSubmdspanSlice, DerivedFromFullExtent) {
  struct derived_from_full_extent_t : full_extent_t {};
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, derived_from_full_extent_t>);
}

/// @test @c is_submdspan_slice_v is @c true for a user defined index-pair-like
TEST(IsSubmdspanSlice, UserDefinedPairLike) {
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<int, int_pair_like>);
  STATIC_ASSERT_TRUE(is_submdspan_slice_v<std::size_t, int_pair_like>);
}

/// @test @c is_submdspan_slice_v is @c false for references to '*_slice' types
/// @note submdspan slice specifiers are passed by value
TEST(IsSubmdspanSlice, ReferencesToSlices) {
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, extent_slice<int>&>);
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, range_slice<int>&>);
}

/// @test @c is_submdspan_slice_v is @c false for types that derive from '*_slice' types
TEST(IsSubmdspanSlice, DerivedFromSlices) {
  struct derived_from_extent_slice : extent_slice<int> {};
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, derived_from_extent_slice>);

  struct derived_from_range_slice : range_slice<int> {};
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, derived_from_range_slice>);
}

/// @test @c is_submdspan_slice_v is @c false for tuple-likes that are not size 2
TEST(IsSubmdspanSlice, TupleLikesThatAreNotPairLike) {
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, arene::base::array<int, 0>>);
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, arene::base::array<int, 1>>);
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, arene::base::array<int, 3>>);

  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, std::tuple<>>);
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, std::tuple<int>>);
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, std::tuple<int, int, int>>);
}

/// @test @c is_submdspan_slice_v is @c false for types that are explicitly convertible to the index type
TEST(IsSubmdspanSlice, ExplicitlyConvertibleToIndex) {
  struct my_int {
    explicit operator int() const { return 0; }
  };
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, my_int>);
}

/// @test @c is_submdspan_slice_v is @c false for other types
TEST(IsSubmdspanSlice, OtherTypes) {
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, int*>);

  struct my_type {};
  STATIC_ASSERT_FALSE(is_submdspan_slice_v<int, my_type>);
}

}  // namespace
