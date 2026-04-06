// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/submdspan_subextents_type.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace {
using arene::base::dynamic_extent;
using arene::base::extent_slice;
using arene::base::extents;
using arene::base::full_extent;
using arene::base::full_extent_t;
using arene::base::submdspan_subextents_type;
using arene::base::submdspan_subextents_type_t;

struct use_submdspan_subextents_type_t {
  template <class Extents, class... SliceSpecifiers>
  using type = submdspan_subextents_type_t<Extents, SliceSpecifiers...>;
};

struct use_submdspan_subextents_type {
  template <class Extents, class... SliceSpecifiers>
  using type = typename submdspan_subextents_type<Extents, SliceSpecifiers...>::type;
};

template <class F, class... Args>
using apply_metafunction_t = typename F::template type<Args...>;

template <class T>
struct SubmdspanSubextentsType : ::testing::Test {};

using metafunction_types = ::testing::Types<use_submdspan_subextents_type, use_submdspan_subextents_type_t>;

TYPED_TEST_SUITE(SubmdspanSubextentsType, metafunction_types, );

/// @test @c submdspan_subextents_type collapses the rank if a slice is a
/// collapsing slice
TYPED_TEST(SubmdspanSubextentsType, CollapsingSlices) {
  static_assert(
      std::is_same<
          extents<std::size_t, 1, dynamic_extent, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              full_extent_t,
              full_extent_t,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<
          extents<std::size_t, 1, dynamic_extent>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              full_extent_t,
              full_extent_t,
              std::size_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<
          extents<std::size_t, 1, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              full_extent_t,
              std::size_t,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<
          extents<std::size_t, dynamic_extent, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              std::size_t,
              full_extent_t,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<                 //
          extents<std::size_t, 3>,  //
          apply_metafunction_t<     //
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              int,
              int,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<                              //
          extents<std::size_t, dynamic_extent>,  //
          apply_metafunction_t<                  //
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              int,
              full_extent_t,
              int>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<                 //
          extents<std::size_t, 1>,  //
          apply_metafunction_t<     //
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              full_extent_t,
              int,
              int>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<              //
          extents<std::size_t>,  //
          apply_metafunction_t<  //
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              int,
              int,
              int>>::value,
      "must be the same type"
  );
}

/// @test @c submdspan_subextents_type replaces a static extent if the slice
/// type is @c extent_slice
TYPED_TEST(SubmdspanSubextentsType, ExtentSlice) {
  static_assert(
      std::is_same<
          extents<std::size_t, 1, dynamic_extent, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, 2, 3>,
              full_extent_t,
              extent_slice<int, int, int>,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<
          extents<std::size_t, 1, dynamic_extent, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              full_extent_t,
              extent_slice<int, int, int>,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<
          extents<std::size_t, 1, 1, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, 2, 3>,
              full_extent_t,
              extent_slice<int, std::integral_constant<std::size_t, 1>, int>,
              full_extent_t>>::value,
      "must be the same type"
  );

  static_assert(
      std::is_same<
          extents<std::size_t, 1, 1, 3>,
          apply_metafunction_t<
              TypeParam,
              extents<std::size_t, 1, dynamic_extent, 3>,
              full_extent_t,
              extent_slice<int, std::integral_constant<std::size_t, 1>, int>,
              full_extent_t>>::value,
      "must be the same type"
  );
}

/// @test @c subextents_static_extent_for directly for line coverage
TEST(SubmdspanSubextentsType, SubextentsStaticExtentFor) {
  using arene::base::submdspan_subextents_type_detail::subextents_static_extent_for;

  // first argument is returned
  ASSERT_EQ(std::size_t{}, subextents_static_extent_for(std::size_t{}, full_extent));
  ASSERT_EQ(std::size_t{1}, subextents_static_extent_for(std::size_t{1}, full_extent));

  // dynamic_extent is returned if the extent_slice extent is a runtime value
  ASSERT_EQ(
      dynamic_extent,
      subextents_static_extent_for(std::size_t{2}, extent_slice<std::size_t, std::size_t, std::size_t>{0, 0})
  );
  ASSERT_EQ(
      dynamic_extent,
      subextents_static_extent_for(std::size_t{2}, extent_slice<std::size_t, std::size_t, std::size_t>{0, 1})
  );

  // compile-time extent is returned if the extent_slice extent is a compile-time value
  ASSERT_EQ(
      std::size_t{1},
      subextents_static_extent_for(
          std::size_t{2},
          extent_slice<std::size_t, std::integral_constant<std::size_t, 1>, std::size_t>{}
      )
  );
}

}  // namespace
