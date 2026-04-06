// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/submdspan_canonicalize_slices.hpp"

#include <gtest/gtest.h>

#include "arene/base/functional/bind_front.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/tests/submdspan_slices_handling_test_common.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::bind_front;
using ::arene::base::dextents;
using ::arene::base::extent_slice;
using ::arene::base::extents;
using ::arene::base::full_extent;
using ::arene::base::full_extent_t;
using ::arene::base::make_extents;
using ::arene::base::range_slice;
using ::arene::base::submdspan_canonicalize_slices;

// NOLINTBEGIN(readability-identifier-length)
template <int N>
constexpr auto ic = std::integral_constant<int, N>{};
template <std::size_t N>
constexpr auto zc = std::integral_constant<std::size_t, N>{};
// NOLINTEND(readability-identifier-length)

template <class T>
struct zero_constant {
  static constexpr T value{};
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T() const noexcept { return value; }
};

template <class T>
constexpr T zero_constant<T>::value;

template <class T>
struct SubmdspanCanonicalizeSlices : testing::Test {};

using IndexTypes = ::testing::Types<int, std::size_t>;

constexpr struct {
  template <class T1, class T2>
  constexpr auto operator()(T1&& lhs, T2&& rhs) const -> bool {
    return std::is_same<decltype(lhs), decltype(rhs)>::value &&   //
           std::get<0>(lhs).offset == std::get<0>(rhs).offset &&  //
           std::get<0>(lhs).extent == std::get<0>(rhs).extent &&  //
           std::get<0>(lhs).stride == std::get<0>(rhs).stride;
  }
} is_same_1_tuple{};

TYPED_TEST_SUITE(SubmdspanCanonicalizeSlices, IndexTypes, );

///@test types convertible to @c full_extent_t are canonicalized to @c full_extent_t
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, FullExtent) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      make_extents(std::integral_constant<index_type, 3>{})
  );

  static_assert(     //
      std::is_same<  //
          std::tuple<full_extent_t>,
          decltype(canonicalize(full_extent))>::value,
      "incorrect result for canonicalization"
  );

  struct convertible_to_full_extent_and_index_type {
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator full_extent_t() const noexcept { return full_extent; }
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator index_type() const noexcept { return index_type{}; }
  };

  static_assert(     //
      std::is_same<  //
          std::tuple<full_extent_t>,
          decltype(canonicalize(convertible_to_full_extent_and_index_type{}))>::value,
      "incorrect result for canonicalization"
  );

  // for coverage
  std::ignore = canonicalize(full_extent);
  std::ignore = canonicalize(convertible_to_full_extent_and_index_type{});
}

/// @test integral-constant-likes are canonicalized to an @c integral_constant
/// with the @c extents::index_type and with the same @c value
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, ConstantCanonicalIndex) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      extents<index_type, 3>{}
  );

  // for coverage
  std::ignore = std::tuple<index_type>{} == canonicalize(zc<0>);

  static_assert(     //
      std::is_same<  //
          std::tuple<std::integral_constant<index_type, 0>>,
          decltype(canonicalize(zc<0>))>::value,
      "incorrect result for canonicalization"
  );

  static_assert(     //
      std::is_same<  //
          std::tuple<std::integral_constant<index_type, 1>>,
          decltype(canonicalize(zc<1>))>::value,
      "incorrect result for canonicalization"
  );

  static_assert(     //
      std::is_same<  //
          std::tuple<std::integral_constant<index_type, 1>>,
          decltype(canonicalize(ic<1>))>::value,
      "incorrect result for canonicalization"
  );

  static_assert(     //
      std::is_same<  //
          std::tuple<std::integral_constant<index_type, 0>>,
          decltype(canonicalize(zero_constant<std::size_t>{}))>::value,
      "incorrect result for canonicalization"
  );

  static_assert(     //
      std::is_same<  //
          std::tuple<std::integral_constant<index_type, 0>>,
          decltype(canonicalize(zero_constant<int>{}))>::value,
      "incorrect result for canonicalization"
  );
}

/// @test non integral-constant-likes that are convertible to index type are
/// canonicalized to @c index_type
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, IntegerCanonicalIndex) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      extents<index_type, 3>{}
  );

  // for coverage
  std::ignore = std::tuple<index_type>{} == canonicalize(int{});

  CONSTEXPR_ASSERT(  //
      std::tuple<index_type>{} == canonicalize(int{})
  );
  static_assert(     //
      std::is_same<  //
          std::tuple<index_type>,
          decltype(canonicalize(int{}))>::value,
      "incorrect result for canonicalization"
  );

  CONSTEXPR_ASSERT(  //
      std::tuple<index_type>{} == canonicalize(std::size_t{})
  );
  static_assert(     //
      std::is_same<  //
          std::tuple<index_type>,
          decltype(canonicalize(std::size_t{}))>::value,
      "incorrect result for canonicalization"
  );

  CONSTEXPR_ASSERT(  //
      std::tuple<index_type>{} == canonicalize(float{})
  );
  static_assert(     //
      std::is_same<  //
          std::tuple<index_type>,
          decltype(canonicalize(float{}))>::value,
      "incorrect result for canonicalization"
  );

  CONSTEXPR_ASSERT(  //
      std::tuple<index_type>{index_type{1}} == canonicalize(int{1})
  );
  static_assert(     //
      std::is_same<  //
          std::tuple<index_type>,
          decltype(canonicalize(int{1}))>::value,
      "incorrect result for canonicalization"
  );

  CONSTEXPR_ASSERT(  //
      std::tuple<index_type>{index_type{1}} == canonicalize(std::size_t{1})
  );
  static_assert(     //
      std::is_same<  //
          std::tuple<index_type>,
          decltype(canonicalize(std::size_t{1}))>::value,
      "incorrect result for canonicalization"
  );

  CONSTEXPR_ASSERT(  //
      std::tuple<index_type>{index_type{1}} == canonicalize(float{1})
  );
  static_assert(     //
      std::is_same<  //
          std::tuple<index_type>,
          decltype(canonicalize(float{1}))>::value,
      "incorrect result for canonicalization"
  );
}

/// @test @c extent_slice types are canonicalized to @c extent_slice types with
/// the canonical index types
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, ExtentSlice) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      extents<index_type, 5>{}
  );

  // for coverage
  std::ignore = canonicalize(extent_slice<int>{});

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 0>,
          std::integral_constant<index_type, 2>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(extent_slice<  //
                   std::integral_constant<index_type, 0>,
                   std::integral_constant<index_type, 2>,
                   std::integral_constant<index_type, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 0>,
          std::integral_constant<index_type, 5>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(extent_slice<  //
                   std::integral_constant<index_type, 0>,
                   std::integral_constant<index_type, 5>,
                   std::integral_constant<index_type, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 5>,
          std::integral_constant<index_type, 0>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(extent_slice<  //
                   std::integral_constant<index_type, 5>,
                   std::integral_constant<index_type, 0>,
                   std::integral_constant<index_type, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 2>,
          std::integral_constant<index_type, 3>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(extent_slice<  //
                   std::integral_constant<index_type, 2>,
                   std::integral_constant<index_type, 3>,
                   std::integral_constant<index_type, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 0>,
          std::integral_constant<index_type, 2>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(extent_slice<  //
                   zero_constant<index_type>,
                   std::integral_constant<std::size_t, 2>,
                   std::integral_constant<int, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::make_tuple(extent_slice<  //
                      std::integral_constant<index_type, 0>,
                      index_type,
                      index_type>{//
                                  {},
                                  index_type{2},
                                  index_type{1}
      }),
      canonicalize(extent_slice<  //
                   zero_constant<index_type>,
                   std::size_t,
                   int>{//
                        zero_constant<index_type>{},
                        std::size_t{2},
                        int{1}
      })
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::make_tuple(extent_slice<  //
                      index_type,
                      index_type,
                      index_type>{//
                                  0,
                                  2,
                                  1
      }),
      canonicalize(extent_slice<  //
                   bool,
                   std::size_t,
                   int>{//
                        false,
                        std::size_t{2},
                        int{1}
      })
  ));
}

/// @test @c range_slice types are canonicalized to @c extent_slice types with
/// the canonical index types
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, ConvertsRangeSliceToExtentSlice) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      extents<index_type, 5>{}
  );

  // for coverage
  std::ignore = canonicalize(range_slice<decltype(ic<0>), decltype(ic<1>)>{});

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 0>,
          std::integral_constant<index_type, 2>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(range_slice<  //
                   std::integral_constant<index_type, 0>,
                   std::integral_constant<index_type, 2>,
                   std::integral_constant<index_type, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::tuple<extent_slice<
          std::integral_constant<index_type, 0>,
          std::integral_constant<index_type, 2>,
          std::integral_constant<index_type, 1>>>{},
      canonicalize(range_slice<  //
                   zero_constant<index_type>,
                   std::integral_constant<std::size_t, 2>,
                   std::integral_constant<int, 1>>{})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::make_tuple(extent_slice<  //
                      std::integral_constant<index_type, 0>,
                      index_type,
                      index_type>{//
                                  {},
                                  index_type{2},
                                  index_type{1}
      }),
      canonicalize(range_slice<  //
                   zero_constant<index_type>,
                   std::size_t,
                   int>{//
                        zero_constant<index_type>{},
                        std::size_t{2},
                        int{1}
      })
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(
      std::make_tuple(extent_slice<  //
                      index_type,
                      index_type,
                      index_type>{//
                                  0,
                                  2,
                                  1
      }),
      canonicalize(range_slice<  //
                   bool,
                   std::size_t,
                   int>{//
                        false,
                        std::size_t{2},
                        int{1}
      })
  ));
}

/// @test @c range_slice types are canonicalized to @c extent_slice where the @c
/// extent member is calculated from the input @c first, @c last, and @c stride
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, ExtentCalculatedFromRangeSlice) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      extents<index_type, 5>{}
  );

  using extent_slice_type = extent_slice<index_type, index_type, index_type>;
  using range_slice_type = range_slice<index_type, index_type, index_type>;

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 2, 1}},
      canonicalize(range_slice_type{0, 2, 1})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 1, 2}},
      canonicalize(range_slice_type{0, 2, 2})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 0, 1}},
      canonicalize(range_slice_type{0, 0, 1})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 0, 2}},
      canonicalize(range_slice_type{0, 0, 2})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{1, 0, 2}},
      canonicalize(range_slice_type{1, 1, 2})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 1, 2}},
      canonicalize(range_slice_type{0, 1, 2})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 2, 2}},
      canonicalize(range_slice_type{0, 3, 2})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 2, 2}},
      canonicalize(range_slice_type{0, 4, 2})
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      std::tuple<extent_slice_type>{{0, 3, 2}},
      canonicalize(range_slice_type{0, 5, 2})
  ));
}

/// @test index-pair-likes are canonicalized to @c extent_slice where the
/// index-pair-like is treated as a @c range_slice where the first element is @c
/// first and the second element is @c last and with a stride of 1
CONSTEXPR_TYPED_TEST(SubmdspanCanonicalizeSlices, IndexPairLikeInterpretedAsRangeSlice) {
  using index_type = TypeParam;

  auto const canonicalize = bind_front(  //
      submdspan_canonicalize_slices,
      extents<index_type, 5>{}
  );

  // for coverage
  std::ignore = canonicalize(std::make_pair(int{}, int{}));
  std::ignore = canonicalize(std::make_pair(int{}, int{1}));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<decltype(ic<0>), decltype(ic<4>)>{}),
      canonicalize(std::make_pair(ic<0>, ic<4>))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<decltype(zc<0>), decltype(zc<4>)>{}),
      canonicalize(std::make_pair(zc<0>, zc<4>))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<decltype(zc<0>), decltype(zc<4>)>{}),
      canonicalize(std::make_pair(zero_constant<int>{}, zc<4>))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<int, int>{0, 4}),
      canonicalize(std::make_pair(0, 4))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<std::size_t, std::size_t>{2, 4}),
      canonicalize(std::make_pair(2, 4))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<index_type, index_type>{2, 4}),
      canonicalize(std::make_pair(2.0F, 4.0F))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<index_type, index_type>{2, 4}),
      canonicalize(std::make_tuple(2.0F, 4.0F))
  ));

  CONSTEXPR_ASSERT(is_same_1_tuple(  //
      canonicalize(range_slice<index_type, index_type>{0, 0}),
      canonicalize(int_pair_like{})
  ));
}

/// @test order of canonical slices matches input order
TYPED_TEST(SubmdspanCanonicalizeSlices, OutputOrderMatchesInputOrder) {
  using index_type = TypeParam;

  static_assert(  //
      std::is_same<  //
          std::tuple<
              std::integral_constant<index_type, 1>,
              std::integral_constant<index_type, 2>,
              std::integral_constant<index_type, 3>  //
              >,
          decltype(submdspan_canonicalize_slices(  //
              dextents<index_type, 3>{10, 10, 10},
              zc<1>,
              zc<2>,
              zc<3>
          ))>::value
          , "output order must match input order"
  );
}

/// @test @c submdspan_canonicalize_slices is not invocable if the number of
/// slices does not match the rank of the extents
TYPED_TEST(SubmdspanCanonicalizeSlices, InvocableIfSameSize) {
  using index_type = TypeParam;
  static_assert_invocable_if_same_size<decltype(submdspan_canonicalize_slices), index_type>();
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,fuchsia-statically-constructed-objects)
auto const canonicalize_with_extents =
    [](auto... slices) noexcept(  //
        noexcept(arene::base::submdspan_canonicalize_slices(extents<int, 3, 3>{}, std::declval<decltype(slices)>()...))
    ) { return arene::base::submdspan_canonicalize_slices(extents<int, 3, 3>{}, slices...); };

struct test_config {
  static decltype((canonicalize_with_extents)) function_under_test;
};

decltype((canonicalize_with_extents)) test_config::function_under_test = canonicalize_with_extents;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables,fuchsia-statically-constructed-objects)

using test_config_type = ::testing::Types<test_config>;

INSTANTIATE_TYPED_TEST_SUITE_P(SubmdspanCanonicalizeSlices, SubmdspanSliceHandlingCommon, test_config_type, );

/// @test helper @c mandate_each_argtype_is_submdspan_slice returns @c nullptr
TEST(SubmdspanCanoncalizeSlices, MandatesHelpersReturnsNullptPtr) {
  // this is only called at compile-time so we invoke it at runtime for line coverage
  using arene::base::mdspan_detail::mandate_each_argtype_is_submdspan_slice;

  ASSERT_EQ(nullptr, mandate_each_argtype_is_submdspan_slice<int>());
  ASSERT_EQ(nullptr, mandate_each_argtype_is_submdspan_slice<std::size_t>());
}

}  // namespace
