// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/submdspan_extents.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/submdspan_slices_handling_test_common.hpp"  //  IWYU pragma: keep
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/apply.hpp"

namespace {
using ::arene::base::apply;
using ::arene::base::array;
using ::arene::base::bind_front;
using ::arene::base::dextents;
using ::arene::base::dynamic_extent;
using ::arene::base::extent_slice;
using ::arene::base::extents;
using ::arene::base::submdspan_extents;

template <class I>
using uniform_extent_slice = extent_slice<I, I, I>;

template <std::size_t... Is>
using static_extent_slice = extent_slice<std::integral_constant<std::size_t, Is>...>;

template <std::size_t... Is>
using static_range_slice = range_slice<std::integral_constant<std::size_t, Is>...>;

template <std::size_t First, std::size_t Last>
using static_pair_slice =
    std::tuple<std::integral_constant<std::size_t, First>, std::integral_constant<std::size_t, Last>>;

using IndexTypes = ::testing::Types<int, std::size_t>;

template <class T>
struct SubmdspanExtents : testing::Test {};

TYPED_TEST_SUITE(SubmdspanExtents, IndexTypes, );

template <class I>
constexpr auto extents_test_types = std::make_tuple(
    extents<I>{},
    extents<I, 1>{},
    extents<I, 1, 2>{},
    extents<I, 1, 2, 3>{},
    extents<I, 1, dynamic_extent, 3, dynamic_extent>{1, 2, 3, 4},
    dextents<I, 5>{1, 2, 3, 4, 5}
);

template <class T>
struct SubmdspanExtentsParamTest : testing::Test {};

template <class I, std::size_t R>
struct test_case {
  using index_type = I;
  static constexpr auto rank = R;

  static constexpr decltype(std::get<R>(extents_test_types<I>)) extents = std::get<R>(extents_test_types<I>);
};
template <class I, std::size_t R>
constexpr decltype(std::get<R>(extents_test_types<I>)) test_case<I, R>::extents;

using ParamTypes = ::testing::Types<
    test_case<int, 0>,
    test_case<int, 1>,
    test_case<int, 2>,
    test_case<int, 3>,
    test_case<int, 4>,
    test_case<int, 5>,
    test_case<std::size_t, 0>,
    test_case<std::size_t, 1>,
    test_case<std::size_t, 2>,
    test_case<std::size_t, 3>,
    test_case<std::size_t, 4>,
    test_case<std::size_t, 5>>;

TYPED_TEST_SUITE(SubmdspanExtentsParamTest, ParamTypes, );

/// @test @c submdspan_extents with @c full_extent slices returns the same extents
CONSTEXPR_TYPED_TEST(SubmdspanExtentsParamTest, FullExtentSlicesReturnsTheSameExtents) {
  auto subextents = apply(  //
      bind_front(submdspan_extents, TypeParam::extents),
      array<arene::base::testing::convertible_to_full_extent, TypeParam::rank>{}
  );

  CONSTEXPR_ASSERT_EQ(TypeParam::extents, subextents);
  static_assert(     //
      std::is_same<  //
          decltype(TypeParam::extents),
          decltype(subextents) const&>::value,
      "must be same type"
  );
}

/// @test @c submdspan_extents with @c collapsing slices returns a rank-0 extents
CONSTEXPR_TYPED_TEST(SubmdspanExtentsParamTest, CollapsingSlicesReturnsRank0Extents) {
  using index_type = typename TypeParam::index_type;

  {
    auto const all_zeros = array<index_type, TypeParam::rank>{};

    auto subextents = apply(bind_front(submdspan_extents, TypeParam::extents), all_zeros);

    CONSTEXPR_ASSERT_EQ(extents<index_type>{}, subextents);
    static_assert(     //
        std::is_same<  //
            extents<index_type>,
            decltype(subextents)>::value,
        "must be same type"
    );
  }

  {
    auto const all_zeros = array<std::integral_constant<index_type, 0>, TypeParam::rank>{};

    auto subextents = apply(bind_front(submdspan_extents, TypeParam::extents), all_zeros);

    CONSTEXPR_ASSERT_EQ(extents<index_type>{}, subextents);
    static_assert(     //
        std::is_same<  //
            extents<index_type>,
            decltype(subextents)>::value,
        "must be same type"
    );
  }

  {
    auto subextents =
        apply(bind_front(submdspan_extents, TypeParam::extents), arene::base::sequential_values_from<int, 0, TypeParam::rank>);

    CONSTEXPR_ASSERT_EQ(extents<index_type>{}, subextents);
    static_assert(     //
        std::is_same<  //
            extents<index_type>,
            decltype(subextents)>::value,
        "must be same type"
    );
  }
}

/// @test @c submdspan_extents returns a subextent where the value is equal to
/// <c> @c extent_slice.extent </c>, if the slice is an @c extent_slice
CONSTEXPR_TYPED_TEST(SubmdspanExtents, ExtentSliceValue) {
  using index_type = TypeParam;
  constexpr auto rank = 4;
  using test_param = ::test_case<index_type, rank>;

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, rank>{1, 2, 3, 4}),
      submdspan_extents(
          test_param::extents,
          uniform_extent_slice<int>{0, 1, 1},
          uniform_extent_slice<std::size_t>{0, 2, 1},
          uniform_extent_slice<int>{0, 3, 1},
          uniform_extent_slice<std::size_t>{0, 4, 1}
      )
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, rank>{1, 2, 1, 2}),
      submdspan_extents(
          test_param::extents,
          full_extent,
          full_extent,
          uniform_extent_slice<int>{0, 1, 1},
          uniform_extent_slice<std::size_t>{0, 2, 1}
      )
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, rank>{1, 1, 3, 1}),
      submdspan_extents(
          test_param::extents,
          uniform_extent_slice<int>{0, 1, 1},
          static_extent_slice<1, 1, 1>{},
          full_extent,
          uniform_extent_slice<std::size_t>{2, 1, 1}
      )
  );
}

/// @test @c submdspan_extents correctly handles a mixture of collapsing and
/// @c full_extent slices
CONSTEXPR_TYPED_TEST(SubmdspanExtents, MixedCollapsingAndFullExtentSlices) {
  using index_type = TypeParam;
  constexpr auto rank = 4;
  using test_param = ::test_case<index_type, rank>;

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 3>{
          test_param::extents.extent(0),
          test_param::extents.extent(1),
          test_param::extents.extent(2)
      }),
      submdspan_extents(test_param::extents, full_extent, full_extent, full_extent, 0)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 3>{
          test_param::extents.extent(0),
          test_param::extents.extent(1),
          test_param::extents.extent(2)
      }),
      submdspan_extents(test_param::extents, full_extent, full_extent, full_extent, 1)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{test_param::extents.extent(0), test_param::extents.extent(2)}),
      submdspan_extents(test_param::extents, full_extent, 0, full_extent, 0)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{test_param::extents.extent(0), test_param::extents.extent(3)}),
      submdspan_extents(test_param::extents, full_extent, 1, 2, full_extent)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{test_param::extents.extent(1), test_param::extents.extent(2)}),
      submdspan_extents(test_param::extents, 0, full_extent, full_extent, 3)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{test_param::extents.extent(1), test_param::extents.extent(2)}),
      submdspan_extents(test_param::extents, 0, full_extent, full_extent, 0)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{test_param::extents.extent(0), test_param::extents.extent(1)}),
      submdspan_extents(test_param::extents, full_extent, full_extent, 0, 0)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{test_param::extents.extent(2), test_param::extents.extent(3)}),
      submdspan_extents(test_param::extents, 0, 0, full_extent, full_extent)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 1>{test_param::extents.extent(3)}),
      submdspan_extents(test_param::extents, 0, 0, 0, full_extent)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 1>{test_param::extents.extent(3)}),
      submdspan_extents(test_param::extents, 0, 1, 2, full_extent)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 1>{test_param::extents.extent(0)}),
      submdspan_extents(test_param::extents, full_extent, 0, 1, 2)
  );
}

/// @test @c submdspan_extents correctly handles a mixture of collapsing and
/// @c extent_slice slices
CONSTEXPR_TYPED_TEST(SubmdspanExtents, MixedCollapsingAndExtentSliceSlices) {
  using index_type = TypeParam;
  constexpr auto rank = 4;
  using test_param = ::test_case<index_type, rank>;

  using extent_slice = arene::base::extent_slice<index_type, index_type, std::integral_constant<int, 1>>;

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 3>{1, 0, 1}),
      submdspan_extents(test_param::extents, extent_slice{0, 1}, extent_slice{0, 0}, extent_slice{0, 1}, 0)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 3>{1, 2, 0}),
      submdspan_extents(test_param::extents, extent_slice{0, 1}, extent_slice{0, 2}, extent_slice{0, 0}, 0)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 3>{1, 2, 0}),
      submdspan_extents(test_param::extents, extent_slice{0, 1}, extent_slice{0, 2}, extent_slice{0, 0}, 1)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{2, 3}),
      submdspan_extents(test_param::extents, 0, extent_slice{0, 2}, extent_slice{0, 3}, 1)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{2, 1}),
      submdspan_extents(test_param::extents, 0, extent_slice{0, 2}, extent_slice{0, 1}, 1)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 2>{2, 1}),
      submdspan_extents(test_param::extents, 0, extent_slice{0, 2}, 2, extent_slice{0, 1})
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 1>{3}),
      submdspan_extents(test_param::extents, 0, 0, extent_slice{0, 3}, 1)
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 1>{3}),
      submdspan_extents(test_param::extents, 0, 0, 0, extent_slice{0, 3})
  );

  CONSTEXPR_ASSERT_EQ(
      (dextents<index_type, 1>{1}),
      submdspan_extents(test_param::extents, 0, 0, 0, extent_slice{0, 1})
  );
}

/// @test @c submdspan_extents maintains compile-time extents if possible
TYPED_TEST(SubmdspanExtents, KeepsCompileTimeExtents) {
  using index_type = TypeParam;
  constexpr auto rank = 3;
  using test_param = ::test_case<index_type, rank>;

  static_assert(     //
      std::is_same<  //
          extents<index_type, 1, 1, 1>,
          decltype(submdspan_extents(
              test_param::extents,
              static_extent_slice<0, 1, 1>{},
              static_extent_slice<0, 1, 1>{},
              static_extent_slice<0, 1, 1>{}
          ))>::value,
      "must be same type"
  );

  static_assert(     //
      std::is_same<  //
          extents<index_type, 1, 2, 1>,
          decltype(submdspan_extents(
              test_param::extents,
              static_extent_slice<0, 1, 1>{},
              static_extent_slice<0, 2, 1>{},
              static_extent_slice<2, 1, 1>{}
          ))>::value,
      "must be same type"
  );
}

/// @test @c submdspan_extents canonicalizes slices
TYPED_TEST(SubmdspanExtents, CanonicalizesSlices) {
  using index_type = TypeParam;
  constexpr auto rank = 3;
  using test_param = ::test_case<index_type, rank>;

  static_assert(     //
      std::is_same<  //
          extents<index_type, 1, 2, 3>,
          decltype(submdspan_extents(
              test_param::extents,
              static_range_slice<0, 1, 1>{},
              static_range_slice<0, 2, 1>{},
              static_range_slice<0, 3, 1>{}
          ))>::value,
      "must be same type"
  );

  static_assert(     //
      std::is_same<  //
          extents<index_type, 1, 1, 1>,
          decltype(submdspan_extents(
              test_param::extents,
              static_range_slice<0, 1, 1>{},
              static_range_slice<0, 2, 2>{},
              static_range_slice<0, 3, 3>{}
          ))>::value,
      "must be same type"
  );

  static_assert(     //
      std::is_same<  //
          extents<index_type, 1, 1, 1>,
          decltype(submdspan_extents(
              test_param::extents,
              static_range_slice<0, 1, 1>{},
              static_range_slice<1, 2, 1>{},
              static_range_slice<1, 3, 2>{}
          ))>::value,
      "must be same type"
  );

  static_assert(     //
      std::is_same<  //
          extents<index_type, 1, 1, 1>,
          decltype(submdspan_extents(
              test_param::extents,
              static_pair_slice<0, 1>{},
              static_pair_slice<1, 2>{},
              static_pair_slice<2, 3>{}
          ))>::value,
      "must be same type"
  );
}

/// @test @c submdspan_extents is not invocable if the number of
/// slices does not match the rank of the extents
TYPED_TEST(SubmdspanExtents, InvocableIfSameSize) {
  using index_type = TypeParam;
  static_assert_invocable_if_same_size<decltype(submdspan_extents), index_type>();
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,fuchsia-statically-constructed-objects)
auto const canonicalize_with_extents =
    [](auto... slices) noexcept(  //
        noexcept(arene::base::submdspan_extents(extents<int, 3, 3>{}, std::declval<decltype(slices)>()...))
    ) { return arene::base::submdspan_extents(extents<int, 3, 3>{}, slices...); };

struct test_config {
  static decltype((canonicalize_with_extents)) function_under_test;
};

decltype((canonicalize_with_extents)) test_config::function_under_test = canonicalize_with_extents;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables,fuchsia-statically-constructed-objects)

using test_config_type = ::testing::Types<test_config>;

INSTANTIATE_TYPED_TEST_SUITE_P(SubmdspanExtents, SubmdspanSliceHandlingCommon, test_config_type, );

}  // namespace
