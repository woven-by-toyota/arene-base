// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @brief Provides tests for @c tuple_transform
/// @note These tests are not complete; this is not publicly exposed.
///

#include "arene/base/tuple/detail/tuple_reduce.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"

namespace {

using arene::base::array;
using arene::base::tuple_detail::tuple_reduce;

template <class T>
struct TupleReduce : ::testing::Test {};

using test_types = ::testing::Types<  //
    arene::base::array<std::size_t, 0>,
    arene::base::array<std::size_t, 1>,
    arene::base::array<std::size_t, 2>,
    arene::base::array<std::size_t, 3>,
    arene::base::array<std::size_t, 4>  //
    >;

TYPED_TEST_SUITE(TupleReduce, test_types, );

/// @test @c tuple_reduce is not invocable if tuple sizes are different
TYPED_TEST(TupleReduce, ReduceIsEquivalentToProduct) {
  auto values = TypeParam{};
  values.fill(std::size_t{1});

  auto const cvalues = values;

  // NOLINTBEGIN(bugprone-use-after-move)
  ASSERT_EQ(std::size_t{1}, tuple_reduce(values, std::size_t{1}, std::multiplies<>{}));
  ASSERT_EQ(std::size_t{1}, tuple_reduce(std::move(values), std::size_t{1}, std::multiplies<>{}));
  ASSERT_EQ(std::size_t{1}, tuple_reduce(cvalues, std::size_t{1}, std::multiplies<>{}));
  ASSERT_EQ(std::size_t{1}, tuple_reduce(std::move(cvalues), std::size_t{1}, std::multiplies<>{}));
  // NOLINTEND(bugprone-use-after-move)
}

}  // namespace
