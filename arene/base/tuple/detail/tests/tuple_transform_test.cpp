// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @brief Provides tests for @c tuple_transform
/// @note These tests are not complete; this is not publicly exposed.
///

#include "arene/base/tuple/detail/tuple_transform.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/negate.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::array;
using arene::base::tuple_detail::tuple_transform;
using arene::base::tuple_detail::tuple_transform_to_array;
using std::tuple;

/// @test @c tuple_transform is not invocable if tuple sizes are different
TEST(TupleTransform, NotInvocableIfTupleSizesAreDifferent) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          tuple<int>,
          tuple<int, int>,
          std::plus<>>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          tuple<int>,
          tuple<int, int>,
          std::plus<>>,
      "must not be invocable"
  );
}

/// @test @c tuple_transform_to_array is not invocable if the array value type
/// is deduced but the tuple size is zero
TEST(TupleTransform, NotInvocableIfArrayValueDeducedFromZeroElements) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          tuple<>,
          std::negate<>>,
      "must not be invocable"
  );
}

/// @test @c tuple_transform is not invocable with zero tuple input arguments
TEST(TupleTransform, NotInvocableIfNoTupleArgs) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          std::negate<>>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<int>),
          std::negate<>>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          std::negate<>>,
      "must not be invocable"
  );

  auto const no_return_nullary = [] {};

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          decltype(no_return_nullary)>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<int>),
          decltype(no_return_nullary)>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          decltype(no_return_nullary)>,
      "must not be invocable"
  );

  auto const int_return_nullary = []() -> int { return 0; };

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          decltype(int_return_nullary)>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<int>),
          decltype(int_return_nullary)>,
      "must not be invocable"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          decltype(int_return_nullary)>,
      "must not be invocable"
  );
}

/// @test @c tuple_transform is not invocable if the transform is not invocable
/// with the tuple element types
TEST(TupleTransform, NotInvocableIfInvalidTransform) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          tuple<int>,
          std::plus<>>,
      "must not be invocable, transform requires two arguments"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          tuple<int>,
          std::plus<>>,
      "must not be invocable, transform requires two arguments"
  );

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          tuple<int>,
          tuple<int>,
          tuple<int>,
          std::plus<>>,
      "must not be invocable, transform requires two arguments"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          tuple<int>,
          tuple<int>,
          tuple<int>,
          std::plus<>>,
      "must not be invocable, transform requires two arguments"
  );

  struct my_type {};
  auto const needs_rvalue_reference = [](my_type&&) -> bool { return {}; };

  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform),
          tuple<my_type const&>,
          decltype(needs_rvalue_reference)>,
      "must not be invocable, transform requires rvalue reference argument"
  );
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_transform_to_array<>),
          tuple<my_type const&>,
          decltype(needs_rvalue_reference)>,
      "must not be invocable, transform requires rvalue reference argument"
  );
}

/// @test @c tuple_transform with one tuple argument returns a tuple where each element is transformed
CONSTEXPR_TEST(TupleTransform, WithOneTupleArgument) {
  CONSTEXPR_ASSERT_EQ(  //
      std::tuple<int>{-2},
      tuple_transform(tuple<int>{2}, std::negate<>{})
  );
  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 1>{-2}),
      tuple_transform_to_array<>(tuple<int>{2}, std::negate<>{})
  );

  CONSTEXPR_ASSERT_EQ(  //
      std::tuple<int>{-2},
      tuple_transform(array<int, 1>{2}, std::negate<>{})
  );
  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 1>{-2}),
      tuple_transform_to_array<>(array<int, 1>{2}, std::negate<>{})
  );

  CONSTEXPR_ASSERT_EQ(  //
      (std::tuple<int, int>{-2, -3}),
      tuple_transform(tuple<int, int>{2, 3}, std::negate<>{})
  );
  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 2>{-2, -3}),
      tuple_transform_to_array<>(tuple<int, int>{2, 3}, std::negate<>{})
  );
}

/// @test @c tuple_transform with two tuple arguments returns a tuple where each element is
/// transformed by applying to associated elements from the input tuples
CONSTEXPR_TEST(TupleTransform, WithTwoTupleArguments) {
  CONSTEXPR_ASSERT_EQ(  //
      std::tuple<int>{3},
      tuple_transform(tuple<int>{2}, array<int, 1>{1}, std::plus<>{})
  );
  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 1>{3}),
      tuple_transform_to_array<>(tuple<int>{2}, array<int, 1>{1}, std::plus<>{})
  );

  CONSTEXPR_ASSERT_EQ(  //
      (std::tuple<int, int>{3, 4}),
      tuple_transform(tuple<int, int>{2, 0}, array<int, 2>{1, 4}, std::plus<>{})
  );
  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 2>{3, 4}),
      tuple_transform_to_array<>(tuple<int, int>{2, 0}, array<int, 2>{1, 4}, std::plus<>{})
  );
}

/// @test @c tuple_transform_to_array can convert the return type
CONSTEXPR_TEST(TupleTransform, ArrayConvertsReturn) {
  static_assert(
      std::is_same<
          array<int, 1>,
          arene::base::invoke_result_t<  //
              decltype(tuple_transform_to_array<int>),
              array<short, 1>,  // NOLINT(google-runtime-int)
              std::negate<>>>::value,
      "must convert transform return type"
  );

  static_assert(
      std::is_same<
          array<int, 2>,
          arene::base::invoke_result_t<  //
              decltype(tuple_transform_to_array<int>),
              array<short, 2>,  // NOLINT(google-runtime-int)
              std::negate<>>>::value,
      "must convert transform return type"
  );

  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 1>{-2}),
      tuple_transform_to_array<int>(std::tuple<short>{short{2}}, std::negate<>{})
  );

  CONSTEXPR_ASSERT_EQ(  //
      (array<int, 2>{-1, -2}),
      tuple_transform_to_array<int>(std::tuple<short, short>{short{1}, short{2}}, std::negate<>{})
  );
}

/// @test @c tuple_transform does not @c std::decay transform result
/// @note arene::base::identity always returns a reference
TEST(TupleTransform, DoesNotDecayTransformResult) {  //
  static_assert(
      std::is_same<
          std::tuple<int&, int&&, int const&&>,
          arene::base::invoke_result_t<  //
              decltype(tuple_transform),
              std::tuple<int&, int&&, int const>,
              arene::base::identity>>::value,
      "must not decay arguments"
  );
}

/// @test @c tuple_transform does @c std::decay tuple elements
TEST(TupleTransform, ArrayDoesDecayTransformResult) {  //
  static_assert(
      std::is_same<
          array<int, 3>,
          arene::base::invoke_result_t<  //
              decltype(tuple_transform_to_array<>),
              std::tuple<int&, int&&, int const>,
              arene::base::identity>>::value,
      "must decay arguments"
  );
}

/// @test @c tuple_transform is @c noexcept(true) if the constructors for the
/// destination are @c noexcept(true) and the transform does not throw
TEST(TupleTransform, NoexceptIfDestinationConstructorsAreNoexceptAndTransformIsNothrow) {
  auto const nothrow_transform = [](auto const&) noexcept(true) -> bool { return {}; };

  static_assert(  //
      noexcept(tuple_transform(std::declval<tuple<>>(), nothrow_transform)),
      "must be noexcept(true)"
  );

  // NOTE: some C++ standard library implemenations don't have conditional
  // 'noexcept' for the 'std::tuple' constructor
  static_assert(
      std::is_nothrow_constructible<std::tuple<std::tuple<int>>, std::tuple<int>>::value ==
          noexcept(tuple_transform(std::declval<tuple<int>>(), nothrow_transform)),
      "noexcept must match"
  );
  static_assert(
      std::is_nothrow_constructible<  //
          std::tuple<std::tuple<int>, std::tuple<int>>,
          std::tuple<int>,
          std::tuple<int>>::value ==  //
          noexcept(tuple_transform(std::declval<array<int, 2>>(), nothrow_transform)),
      "noexcept must match"
  );

  static_assert(
      noexcept(tuple_transform_to_array<bool>(std::declval<tuple<int>>(), nothrow_transform)),
      "must be noexcept(true)"
  );
  static_assert(
      noexcept(tuple_transform_to_array<>(std::declval<tuple<int>>(), nothrow_transform)),
      "must be noexcept(true)"
  );
  static_assert(
      noexcept(tuple_transform_to_array<>(std::declval<array<int, 2>>(), nothrow_transform)),
      "must be noexcept(true)"
  );
}

/// @test @c tuple_transform is @c noexcept(false) if the transform can throw
/// and the tuple argument is not size 0
TEST(TupleTransform, NotNoexceptIfTransformCanThrowAndNotSize0Tuple) {
  auto const throwing_transform = [](auto const&) noexcept(false) -> bool { return {}; };

  static_assert(  //
      !noexcept(tuple_transform(std::declval<tuple<int>>(), throwing_transform)),
      "must be noexcept(false)"
  );
  static_assert(  //
      !noexcept(tuple_transform(std::declval<array<int, 2>>(), throwing_transform)),
      "must be noexcept(false)"
  );

  static_assert(
      !noexcept(tuple_transform_to_array<>(std::declval<tuple<int>>(), throwing_transform)),
      "must be noexcept(false)"
  );
  static_assert(
      !noexcept(tuple_transform_to_array<>(std::declval<array<int, 2>>(), throwing_transform)),
      "must be noexcept(false)"
  );
}

/// @test @c tuple_transform is @c noexcept(true) if the transform can throw
/// and the tuple argument is size 0
TEST(TupleTransform, NoexceptIfTransformCanThrowAndSize0Tuple) {
  auto const throwing_transform = [](auto const&) noexcept(false) -> bool { return {}; };

  static_assert(  //
      noexcept(tuple_transform(std::declval<tuple<>>(), throwing_transform)),
      "must be noexcept(true)"
  );
  static_assert(
      noexcept(tuple_transform_to_array<bool>(std::declval<array<int, 0>>(), throwing_transform)),
      "must be noexcept(true)"
  );
}

}  // namespace
