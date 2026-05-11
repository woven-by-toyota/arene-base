// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @brief Provides tests for @c tuple_transform_reduce
/// @note These tests are not complete; this is not publicly exposed.
///

#include "arene/base/tuple/detail/tuple_transform_reduce.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/negate.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::tuple_detail::tuple_transform_reduce;

/// @test @c tuple_transform_reduce is not invocable if tuple sizes are different
TEST(TupleTransformReduce, NotInvocableIfTupleSizesAreDifferent) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<int>,
          std::tuple<int, int>,
          int,
          std::plus<>,
          std::multiplies<>>,
      "must not be invocable"
  );
}

/// @test @c tuple_transform_reduce is not invocable with zero tuple input arguments
TEST(TupleTransformReduce, NotInvocableIfNoTupleArgs) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          int,
          std::plus<>,
          std::negate<>>,
      "must not be invocable"
  );
}

/// @test @c tuple_transform_reduce is not invocable if the transform is not invocable
/// with the tuple element types
TEST(TupleTransformReduce, NotInvocableIfInvalidTransform) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<int>,
          int,
          std::plus<>,
          std::multiplies<>>,
      "must not be invocable, transform requires two arguments"
  );

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<int>,
          std::tuple<int>,
          std::tuple<int>,
          int,
          std::plus<>,
          std::multiplies<>>,
      "must not be invocable, transform requires two arguments"
  );

  struct my_type {};
  auto const needs_rvalue_reference = [](my_type&&) -> bool { return {}; };

  auto const any_reduce = [](auto const&, auto const&) -> int { return {}; };

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<my_type const&>,
          int,
          decltype(any_reduce),
          decltype(needs_rvalue_reference)>,
      "must not be invocable, transform requires rvalue reference argument"
  );
}

/// @test @c tuple_transform_reduce is not invocable if the reduce is not invocable
/// with the initial value or transform result types
TEST(TupleTransformReduce, NotInvocableIfInvalidReduce) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<int>,
          int,
          std::negate<>,
          std::negate<>>,
      "must not be invocable, reduce requires two arguments"
  );

  struct my_type {};
  auto const bad_reduce = [](my_type const&, auto const&) -> my_type { return {}; };

  static_assert(
      arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<int>,
          my_type,
          decltype(bad_reduce),
          arene::base::identity>,
      "must be invocable"
  );

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_reduce),
          std::tuple<my_type const&>,
          int,
          decltype(bad_reduce),
          arene::base::identity>,
      "must not be invocable, wrong reduce argument order"
  );
}

/// @test @c tuple_transform_reduce can be used with one tuple argument
CONSTEXPR_TEST(TupleTransformReduce, WithOneTupleArgument) {
  struct positive {
    constexpr auto operator()(int value) -> int { return static_cast<int>(value > 0); }
  };

  CONSTEXPR_ASSERT_EQ(  //
      2,
      tuple_transform_reduce(  //
          std::make_tuple(2, 1, 0),
          0,
          std::plus<>{},
          positive{}
      )
  );

  CONSTEXPR_ASSERT_EQ(  //
      2,
      tuple_transform_reduce(  //
          arene::base::to_array({0, 1, 2}),
          0,
          std::plus<>{},
          positive{}
      )
  );
}

/// @test @c tuple_transform_reduce can be used with two tuple arguments
CONSTEXPR_TEST(TupleTransformReduce, WithTwoTupleArguments) {
  CONSTEXPR_ASSERT_EQ(  //
      0 + (1 * 1) + (2 * 2),
      tuple_transform_reduce(
          std::make_tuple(0, 1, 2),
          arene::base::to_array({0, 1, 2}),
          0,
          std::plus<>{},
          std::multiplies<>{}
      )
  );
}

}  // namespace
