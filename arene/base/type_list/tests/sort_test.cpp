// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/sort.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/size.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {

// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

template <typename, typename>
struct null_comparator {
  static constexpr bool value = false;
};

template <typename T1, typename T2>
struct value_comparator {
  static constexpr bool value = T1::value < T2::value;
};

template <std::size_t Value, std::size_t Identity>
struct sortable_element {
  static constexpr std::size_t value = Value;
  static constexpr std::size_t identity = Identity;
};

/// @test `sort_t` does nothing on empty type lists.
TEST(Sort, SortingAnEmptyListDoesNothing) {
  STATIC_ASSERT_TRUE((std::is_same<type_list<>, arene::base::type_lists::sort_t<type_list<>, null_comparator>>::value));
  STATIC_ASSERT_TRUE((std::is_same<std::tuple<>, arene::base::type_lists::sort_t<std::tuple<>, null_comparator>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_same<type_list<>, arene::base::type_lists::sort<type_list<>, null_comparator>::type>::value)
  );
}

/// @test `sort_t` does nothing on single-element type lists.
TEST(Sort, SortingASingleItemDoesNothing) {
  STATIC_ASSERT_TRUE(
      (std::is_same<
          type_list<std::integral_constant<int, 42>>,
          arene::base::type_lists::sort_t<type_list<std::integral_constant<int, 42>>, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_same<
          std::tuple<std::integral_constant<int, 42>>,
          arene::base::type_lists::sort_t<std::tuple<std::integral_constant<int, 42>>, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_same<
          type_list<std::integral_constant<int, 42>>,
          arene::base::type_lists::sort<type_list<std::integral_constant<int, 42>>, value_comparator>::type>::value)
  );
}

/// @test `sort_t` correctly sorts two-element type lists.
TEST(Sort, SortingTwoItemsReturnsThemInOrder) {
  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
                      arene::base::type_lists::sort_t<
                          type_list<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE(
      (std::is_same<
          std::tuple<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
          arene::base::type_lists::sort_t<
              std::tuple<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
              value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
                      arene::base::type_lists::sort<
                          type_list<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
                          value_comparator>::type>::value));

  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
                      arene::base::type_lists::sort_t<
                          type_list<std::integral_constant<std::uint32_t, 2>, std::integral_constant<std::uint32_t, 1>>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE(
      (std::is_same<
          std::tuple<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
          arene::base::type_lists::sort_t<
              std::tuple<std::integral_constant<std::uint32_t, 2>, std::integral_constant<std::uint32_t, 1>>,
              value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<std::integral_constant<std::uint32_t, 1>, std::integral_constant<std::uint32_t, 2>>,
                      arene::base::type_lists::sort<
                          type_list<std::integral_constant<std::uint32_t, 2>, std::integral_constant<std::uint32_t, 1>>,
                          value_comparator>::type>::value));
}

/// @test `sort_t` correctly sorts three-element type lists.
TEST(Sort, SortingThreeItemsReturnsThemInOrder) {
  // A sorted type list
  using sorted_type_list = type_list<
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>>;
  // The same sorted type list as a tuple, representative of an alternative type-list type
  using sorted_tuple = arene::base::type_lists::apply_all_t<sorted_type_list, std::tuple>;
  // All unsorted permutations of the sorted type list
  using unsorted_permutation_1 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 3>>;
  using unsorted_permutation_2 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_3 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_4 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>>;
  using unsorted_permutation_5 = type_list<
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 2>>;

  // For each permutation, check that the result of sorting it is the sorted list, both for type lists, and for typles
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<sorted_type_list, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((std::is_same<sorted_tuple, arene::base::type_lists::sort_t<sorted_tuple, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_1, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_1, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_2, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_2, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_3, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_3, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_4, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_4, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_5, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_5, std::tuple>, value_comparator>>::value
  ));
}

/// @test `sort_t` correctly sorts three-element type lists, while preserving the relative order of equivalent types.
TEST(Sort, SortingThreeItemsReturnsThemInOrderPreservingRelativeOrderForEquivalentTypes) {
  using X1 = sortable_element<10, 1>;
  using X2 = sortable_element<10, 2>;
  using X3 = sortable_element<20, 3>;

  STATIC_ASSERT_TRUE(
      (std::is_same<type_list<X1, X2, X3>, arene::base::type_lists::sort_t<type_list<X1, X2, X3>, value_comparator>>::
           value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_same<type_list<X1, X2, X3>, arene::base::type_lists::sort_t<type_list<X3, X1, X2>, value_comparator>>::
           value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_same<type_list<X1, X2, X3>, arene::base::type_lists::sort_t<type_list<X1, X3, X2>, value_comparator>>::
           value)
  );
}

/// @test `sort_t` correctly sorts four-element type lists.
TEST(Sort, SortingFourItemsReturnsThemInOrder) {
  // A sorted type list
  using sorted_type_list = type_list<
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 4>>;
  // The same sorted type list as a tuple, representative of an alternative type-list type
  using sorted_tuple = arene::base::type_lists::apply_all_t<sorted_type_list, std::tuple>;
  // All unsorted permutations of the sorted type list
  using unsorted_permutation_1 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 4>>;
  using unsorted_permutation_2 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 4>>;
  using unsorted_permutation_3 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 4>>;
  using unsorted_permutation_4 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 4>>;
  using unsorted_permutation_5 = type_list<
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 4>>;
  using unsorted_permutation_6 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_7 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_8 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_9 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_10 = type_list<
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_11 = type_list<
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>>;
  using unsorted_permutation_12 = type_list<
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 3>>;
  using unsorted_permutation_13 = type_list<
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>>;
  using unsorted_permutation_14 = type_list<
      std::integral_constant<std::uint32_t, 2>,
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 3>>;
  using unsorted_permutation_15 = type_list<
      std::integral_constant<std::uint32_t, 3>,
      std::integral_constant<std::uint32_t, 4>,
      std::integral_constant<std::uint32_t, 1>,
      std::integral_constant<std::uint32_t, 2>>;

  // For each permutation, check that the result of sorting it is the sorted list, both for type lists, and for typles
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<sorted_type_list, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((std::is_same<sorted_tuple, arene::base::type_lists::sort_t<sorted_tuple, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_1, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_1, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_2, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_2, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_3, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_3, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_4, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_4, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_5, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_5, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_6, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_6, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_7, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_7, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_8, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_8, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_9, value_comparator>>::value)
  );
  STATIC_ASSERT_TRUE((
      std::is_same<
          sorted_tuple,
          arene::base::type_lists::
              sort_t<arene::base::type_lists::apply_all_t<unsorted_permutation_9, std::tuple>, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((
      std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_10, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((std::is_same<
                      sorted_tuple,
                      arene::base::type_lists::sort_t<
                          arene::base::type_lists::apply_all_t<unsorted_permutation_10, std::tuple>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE((
      std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_11, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((std::is_same<
                      sorted_tuple,
                      arene::base::type_lists::sort_t<
                          arene::base::type_lists::apply_all_t<unsorted_permutation_11, std::tuple>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE((
      std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_12, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((std::is_same<
                      sorted_tuple,
                      arene::base::type_lists::sort_t<
                          arene::base::type_lists::apply_all_t<unsorted_permutation_12, std::tuple>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE((
      std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_13, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((std::is_same<
                      sorted_tuple,
                      arene::base::type_lists::sort_t<
                          arene::base::type_lists::apply_all_t<unsorted_permutation_13, std::tuple>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE((
      std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_14, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((std::is_same<
                      sorted_tuple,
                      arene::base::type_lists::sort_t<
                          arene::base::type_lists::apply_all_t<unsorted_permutation_14, std::tuple>,
                          value_comparator>>::value));
  STATIC_ASSERT_TRUE((
      std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_permutation_15, value_comparator>>::value
  ));
  STATIC_ASSERT_TRUE((std::is_same<
                      sorted_tuple,
                      arene::base::type_lists::sort_t<
                          arene::base::type_lists::apply_all_t<unsorted_permutation_15, std::tuple>,
                          value_comparator>>::value));
}

/// @test `sort_t` correctly sorts four-element type lists, while preserving the relative order of equivalent types.
TEST(Sort, SortingFourItemsReturnsThemInOrderPreservingRelativeOrderForEquivalentTypes) {
  using X1 = sortable_element<10, 1>;
  using X2 = sortable_element<10, 2>;
  using X3 = sortable_element<10, 3>;
  using X4 = sortable_element<30, 4>;

  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<X1, X2, X3, X4>,
                      arene::base::type_lists::sort_t<type_list<X1, X2, X3, X4>, value_comparator>>::value));
  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<X1, X2, X3, X4>,
                      arene::base::type_lists::sort_t<type_list<X1, X2, X4, X3>, value_comparator>>::value));
  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<X1, X2, X3, X4>,
                      arene::base::type_lists::sort_t<type_list<X1, X4, X2, X3>, value_comparator>>::value));
  STATIC_ASSERT_TRUE((std::is_same<
                      type_list<X1, X2, X3, X4>,
                      arene::base::type_lists::sort_t<type_list<X4, X1, X2, X3>, value_comparator>>::value));
}

/// @test `sort_t` correctly sorts multiple-element type lists, while preserving the relative order of equivalent types.
TEST(Sort, SortingMultipleItemsReturnsThemInOrderPreservingRelativeOrderForEquivalentTypes) {
  using X1 = sortable_element<123, 1>;
  using X2 = sortable_element<42, 2>;
  using X3 = sortable_element<23, 3>;
  using X4 = sortable_element<53, 4>;
  using X5 = sortable_element<5, 5>;
  using X6 = sortable_element<99, 6>;
  using X7 = sortable_element<123, 7>;
  using X8 = sortable_element<99, 8>;
  using X9 = sortable_element<123, 9>;
  using X10 = sortable_element<123, 10>;
  using X11 = sortable_element<10254, 11>;
  using X12 = sortable_element<99999, 12>;
  using X13 = sortable_element<123, 13>;
  using X14 = sortable_element<23, 14>;
  using X15 = sortable_element<24, 15>;
  using X16 = sortable_element<12, 16>;
  using X17 = sortable_element<1, 17>;
  using X18 = sortable_element<1, 18>;
  using X19 = sortable_element<5, 19>;
  using X20 = sortable_element<17, 20>;

  using sorted_type_list =
      type_list<X17, X18, X5, X19, X16, X20, X3, X14, X15, X2, X4, X6, X8, X1, X7, X9, X10, X13, X11, X12>;
  using unsorted_type_list =
      type_list<X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20>;
  static_assert(arene::base::type_lists::size_v<sorted_type_list> == 20, "Right size");
  static_assert(arene::base::type_lists::size_v<unsorted_type_list> == 20, "Right size");
  STATIC_ASSERT_TRUE(
      (std::is_same<sorted_type_list, arene::base::type_lists::sort_t<unsorted_type_list, value_comparator>>::value)
  );
}
}  // namespace
