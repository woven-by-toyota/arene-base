// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/detail/tests/index_permutation_range_test_types.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"

namespace {

using ::arene::base::detail::index_permutation_range;

template <class T>
struct DetailIndexPermutationRangeDeathTest : ::testing::Test {};

TYPED_TEST_SUITE(DetailIndexPermutationRangeDeathTest, ::test::extents_types, );

/// @test dereferencing an @c index_permutation_range::iterator with nullptr
/// range (e.g., if it was default constructed) is a precondition violation
TYPED_TEST(DetailIndexPermutationRangeDeathTest, DereferenceWithNullptrRange) {
  using iterator =
      typename index_permutation_range<typename TypeParam::value_type, std::tuple_size<TypeParam>::value>::iterator;

  ASSERT_DEATH(*iterator{}, "Precondition");
}

/// @test incrementing an @c index_permutation_range::iterator with nullptr
/// range (e.g., if it was default constructed) is a precondition violation
TYPED_TEST(DetailIndexPermutationRangeDeathTest, IncrementWithNullptrRange) {
  using iterator =
      typename index_permutation_range<typename TypeParam::value_type, std::tuple_size<TypeParam>::value>::iterator;

  ASSERT_DEATH(++iterator{}, "Precondition");
}

}  // namespace
