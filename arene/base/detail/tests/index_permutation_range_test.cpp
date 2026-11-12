// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/detail/index_permutation_range.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/reduce.hpp"
// IWYU pragma: no_include "arene/base/array/array.hpp"
#include "arene/base/detail/tests/index_permutation_range_test_types.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace {

using ::arene::base::detail::make_index_permutation_range;

template <class T>
struct DetailIndexPermutationRange : ::testing::Test {};

TYPED_TEST_SUITE(DetailIndexPermutationRange, ::test::extents_types, );

/// @test @c index_permutation_range has no elements if any extents value is
/// zero or extents has rank 0
TYPED_TEST(DetailIndexPermutationRange, NoElementsIfExtentsAreZeroOrZeroRank) {
  auto const extents = TypeParam{};  //  default constructed array

  auto range = make_index_permutation_range(extents);
  auto first = range.begin();
  auto last = range.end();

  ASSERT_EQ(first, last);
}

/// @test the number of elements in @c index_permutation_range is equal to the
/// extents product if rank is greater than 0
TYPED_TEST(DetailIndexPermutationRange, NumberOfElementsEqualsExtentsProduct) {
  using value_type = typename TypeParam::value_type;

  auto extents = TypeParam{};
  extents.fill(2U);

  auto range = make_index_permutation_range(extents);
  auto first = range.begin();
  auto last = range.end();

  if (extents.empty()) {
    ASSERT_EQ(first, last);
  } else {
    ASSERT_TRUE(arene::base::cmp_equal(  //
        arene::base::reduce(extents, value_type{1}, std::multiplies<>{}),
        arene::base::distance(first, last)
    ));
  }
}

/// @test @c index_permutation_range cannot be constructed from a temporary
TYPED_TEST(DetailIndexPermutationRange, NotConstructibleFromTemporary) {
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(make_index_permutation_range), TypeParam>));
}

}  // namespace
