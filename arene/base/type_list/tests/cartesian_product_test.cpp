// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/cartesian_product.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/reduce.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using arene::base::type_lists::cartesian_product_as_t;
using arene::base::type_lists::cartesian_product_t;

template <class... Tn>
using abtl = ::arene::base::type_list<Tn...>;

template <class... Tn>
using gttl = ::testing::Types<Tn...>;

template <class T, std::size_t N>
auto assert_equal_permutations(arene::base::array<T, N> const& actual, arene::base::array<T, N> const& expected)
    -> bool {
  return actual == expected;
}

// NOLINTBEGIN(google-runtime-int) We're not doing math with these types, just using the ones that are easy to read

/// @test A sanity check for some implementation detail functions used by `cartesian_product_t`
/// @note We check a number of permutations for coverage
TEST(TypeListCartesianProductTest, ImplementationDetailCheck) {
  using arene::base::type_lists::cartesian_product_detail::all_index_permutations_impl;
  using arene::base::type_lists::cartesian_product_detail::all_index_permutations_v;
  using arene::base::type_lists::cartesian_product_detail::nth_type_permutation_t;

  ASSERT_TRUE(assert_equal_permutations(
      all_index_permutations_impl<3>(),
      {{
          {0},
          {1},
          {2},
      }}
  ));

  ASSERT_TRUE(assert_equal_permutations(
      all_index_permutations_impl<3, 2>(),
      {{
          {0, 0},
          {0, 1},
          {1, 0},
          {1, 1},
          {2, 0},
          {2, 1},
      }}
  ));

  ASSERT_TRUE(assert_equal_permutations(
      all_index_permutations_impl<2, 2, 1, 3>(),
      {{
          {0, 0, 0, 0},
          {0, 0, 0, 1},
          {0, 0, 0, 2},
          {0, 1, 0, 0},
          {0, 1, 0, 1},
          {0, 1, 0, 2},
          {1, 0, 0, 0},
          {1, 0, 0, 1},
          {1, 0, 0, 2},
          {1, 1, 0, 0},
          {1, 1, 0, 1},
          {1, 1, 0, 2},
      }}
  ));

  ::testing::StaticAssertTypeEq<nth_type_permutation_t<abtl, 0, abtl<int, char, float>>, abtl<int>>();
  ::testing::StaticAssertTypeEq<nth_type_permutation_t<abtl, 1, abtl<int, char, float>>, abtl<char>>();
  ::testing::StaticAssertTypeEq<nth_type_permutation_t<abtl, 2, abtl<int, char, float>>, abtl<float>>();

  // we simply invoke the following for test coverage
  std::ignore = all_index_permutations_impl<39, 2>();
  std::ignore = all_index_permutations_impl<39, 8>();
  std::ignore = all_index_permutations_impl<8, 2>();
  std::ignore = all_index_permutations_impl<2, 2>();
  std::ignore = all_index_permutations_impl<4, 4, 4>();
  std::ignore = all_index_permutations_impl<4, 2>();
  std::ignore = all_index_permutations_impl<2, 2, 2>();
}

/// @test `cartesian_product_t` of no lists is an empty arene::base::type_list
TEST(TypeListCartesianProductTest, ProductOfNothingIsAnEmptyTypeList) {
  ::testing::StaticAssertTypeEq<cartesian_product_t<>, abtl<>>();
}

/// @test `cartesian_product_t` of a single list is a list of that list template, each containing one input type
TEST(TypeListCartesianProductTest, ProductOfASingleListIsAListOfListsOfItsElements) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_t<abtl<int, char, float>>,  //
      abtl<abtl<int>, abtl<char>, abtl<float>>      //
      >();

  ::testing::StaticAssertTypeEq<
      cartesian_product_t<gttl<double, long, short const>>,  //
      gttl<gttl<double>, gttl<long>, gttl<short const>>      //
      >();
}

/// @test `cartesian_product_t` of two type lists is a list of the first list template, each containing two input types
TEST(TypeListCartesianProductTest, ProductOfTwoListsIsAllCombinationsOfThem) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_t<abtl<int, char>, abtl<float, void>>,                      //
      abtl<abtl<int, float>, abtl<int, void>, abtl<char, float>, abtl<char, void>>  //
      >();

  ::testing::StaticAssertTypeEq<
      cartesian_product_t<gttl<void, void const>, gttl<void volatile, void>>,                                     //
      gttl<gttl<void, void volatile>, gttl<void, void>, gttl<void const, void volatile>, gttl<void const, void>>  //
      >();
}

/// @test `cartesian_product_t` of two type lists with different list templates uses the first one
TEST(TypeListCartesianProductTest, ProductOfTwoListsWithDifferentListTypesUsesTheFirstOne) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_t<abtl<int, char>, gttl<float, void>>,                      //
      abtl<abtl<int, float>, abtl<int, void>, abtl<char, float>, abtl<char, void>>  //
      >();

  ::testing::StaticAssertTypeEq<
      cartesian_product_t<gttl<int, char>, abtl<float, void>>,                      //
      gttl<gttl<int, float>, gttl<int, void>, gttl<char, float>, gttl<char, void>>  //
      >();
}

/// @test `cartesian_product_t` of three type lists is a list of the first list template, with all combinations
TEST(TypeListCartesianProductTest, ProductOfThreeListsIsAllCombinationsOfThem) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_t<gttl<int, long>, abtl<void, void*>, abtl<float, double>>,
      gttl<
          gttl<int, void, float>,
          gttl<int, void, double>,
          gttl<int, void*, float>,
          gttl<int, void*, double>,
          gttl<long, void, float>,
          gttl<long, void, double>,
          gttl<long, void*, float>,
          gttl<long, void*, double>>>();
}

/// @test `cartesian_product_t` including any empty type lists is an empty specialization of the first list template
TEST(TypeListCartesianProductTest, AnyListBeingEmptyResultsInAnEmptyOutput) {
  ::testing::
      StaticAssertTypeEq<cartesian_product_t<gttl<int, long>, abtl<void, void*>, gttl<>, abtl<float, double>>, gttl<>>(
      );
}

/// @test `cartesian_product_as_t` of no lists is an empty arene::base::type_list
TEST(TypeListCartesianProductTest, TypeSwappedProductOfNothingIsAnEmptyTypeList) {
  ::testing::StaticAssertTypeEq<cartesian_product_as_t<gttl>, abtl<>>();
}

/// @test `cartesian_product_as_t` of a single list is a list of the "as" template, each containing one input type
TEST(TypeListCartesianProductTest, TypeSwappedProductOfASingleListIsAListOfListsOfItsElements) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<gttl, abtl<int, char, float>>,  //
      abtl<gttl<int>, gttl<char>, gttl<float>>               //
      >();

  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<abtl, gttl<double, long, short const>>,  //
      gttl<abtl<double>, abtl<long>, abtl<short const>>               //
      >();
}

/// @test `cartesian_product_as_t` of two type lists is a list of the "as" template, each containing two input types
TEST(TypeListCartesianProductTest, TypeSwappedProductOfTwoListsIsAllCombinationsOfThem) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<gttl, abtl<int, char>, abtl<float, void>>,             //
      abtl<gttl<int, float>, gttl<int, void>, gttl<char, float>, gttl<char, void>>  //
      >();

  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<abtl, gttl<void, void const>, gttl<void volatile, void>>,                            //
      gttl<abtl<void, void volatile>, abtl<void, void>, abtl<void const, void volatile>, abtl<void const, void>>  //
      >();
}

/// @test `cartesian_product_as_t` of two type lists with different list templates uses the first one for the outer list
TEST(TypeListCartesianProductTest, TypeSwappedProductOfTwoListsWithDifferentListTypesUsesTheFirstOne) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<gttl, abtl<int, char>, gttl<float, void>>,             //
      abtl<gttl<int, float>, gttl<int, void>, gttl<char, float>, gttl<char, void>>  //
      >();

  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<abtl, gttl<int, char>, abtl<float, void>>,             //
      gttl<abtl<int, float>, abtl<int, void>, abtl<char, float>, abtl<char, void>>  //
      >();
}

/// @test `cartesian_product_as_t` of three type lists is a list of the "as" template, with all combinations
TEST(TypeListCartesianProductTest, TypeSwappedProductOfThreeListsIsAllCombinationsOfThem) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<abtl, gttl<int, long>, abtl<void, void*>, abtl<float, double>>,
      gttl<
          abtl<int, void, float>,
          abtl<int, void, double>,
          abtl<int, void*, float>,
          abtl<int, void*, double>,
          abtl<long, void, float>,
          abtl<long, void, double>,
          abtl<long, void*, float>,
          abtl<long, void*, double>>>();
}

/// @test `cartesian_product_as_t` including any empty type lists is an empty specialization of the first list template
TEST(TypeListCartesianProductTest, TypeSwappedAnyListBeingEmptyResultsInAnEmptyOutput) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<abtl, gttl<int, long>, abtl<void, void*>, gttl<>, abtl<float, double>>,
      gttl<>>();
}

/// @test `cartesian_product_as_t` with a non-variadic "as" parameter can successfully return a list of that parameter
TEST(TypeListCartesianProductTest, CartesianProductAsWorksWithNonVariadicTemplates) {
  ::testing::StaticAssertTypeEq<
      cartesian_product_as_t<std::pair, gttl<int, long, double>, abtl<void*, int*>>,
      gttl<
          std::pair<int, void*>,
          std::pair<int, int*>,
          std::pair<long, void*>,
          std::pair<long, int*>,
          std::pair<double, void*>,
          std::pair<double, int*>>>();
}

// NOLINTEND(google-runtime-int)

/// @test exercise the @c reduce instantiation used at compile time by
///   @c permutation_space_size_v at run time
///
/// @c permutation_space_size_v invokes @c arene::base::reduce over a
/// @c std::initializer_list<std::size_t> with @c std::multiplies<> only in a
/// constant expression, so the body of that @c reduce instantiation is never
/// executed at run time. Every translation unit that includes
/// @c cartesian_product.hpp emits this instantiation, which drags the merged
/// line coverage of @c reduce.hpp and @c generalized_reduce.hpp below 100%.
/// Calling the exact same instantiation at run time here covers those lines.
TEST(TypeListCartesianProductTest, PermutationSpaceSizeReduceIsCoveredAtRuntime) {
  ASSERT_EQ(
      24U,
      arene::base::reduce(std::initializer_list<std::size_t>{2U, 3U, 4U}, std::size_t{1}, std::multiplies<>{})
  );
}

}  // namespace
