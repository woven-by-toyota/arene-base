// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/algorithm"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace iter_swap_test {

struct type_with_swap_specialization {
  constexpr explicit type_with_swap_specialization() = default;

  bool swapped{false};
};

struct type_with_iter_swap_specialization {
  constexpr explicit type_with_iter_swap_specialization() = default;

  bool swapped{false};
};
}  // namespace iter_swap_test

namespace std {
template <>
constexpr void swap(
    ::iter_swap_test::type_with_swap_specialization& lhs,
    ::iter_swap_test::type_with_swap_specialization& rhs
) noexcept {
  lhs.swapped = true;
  rhs.swapped = true;
}

template <>
constexpr void iter_swap(
    ::iter_swap_test::type_with_iter_swap_specialization* lhs,
    ::iter_swap_test::type_with_iter_swap_specialization* rhs
) noexcept {
  lhs->swapped = true;
  rhs->swapped = true;
}

}  // namespace std

namespace {

namespace detail {
struct type_with_swap_fn {
  constexpr explicit type_with_swap_fn() = default;

  bool swapped{false};
};

constexpr auto swap(type_with_swap_fn& lhs, type_with_swap_fn& rhs) {
  lhs.swapped = true;
  rhs.swapped = true;
}

}  // namespace detail

using iter_swap_types = ::testing::integral_types;

template <typename T>
class IterSwapTest : public testing::Test {};

TYPED_TEST_SUITE(IterSwapTest, iter_swap_types, );

/// @test @c std::iter_swap should always return void
TYPED_TEST(IterSwapTest, IterSwapHasCorrectReturnType) {
  testing::StaticAssertTypeEq<decltype(std::iter_swap(std::declval<TypeParam*>(), std::declval<TypeParam*>())), void>();
}

/// @test @c std::iter_swap is not invocable if one of the inputs is not a forward_iterator iterator
TEST(IterSwapTest, IterSwapIsCorrectlyConstrainedIfNotGivenIterator) {
  auto lifted_iter_swap = FUNCTION_LIFT(std::iter_swap);

  using forward = testing::demoted_iterator<int*, std::forward_iterator_tag>;
  using input = testing::demoted_iterator<int*, std::input_iterator_tag>;
  using output = testing::demoted_iterator<int*, std::output_iterator_tag>;
  using bidirectional = testing::demoted_iterator<int*, std::bidirectional_iterator_tag>;

  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), input, input>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), output, output>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), forward, forward>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), bidirectional, bidirectional>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), int*, int*>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), int, int*>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_iter_swap), int*, int>);
}

/// @test @c std::iter_swap is not invocable if the objects pointed to are not swappable
TEST(IterSwapTest, IterSwapIsCorrectlyConstrainedIfSwappable) {
  auto lifted_iter_swap = FUNCTION_LIFT(std::iter_swap);

  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_iter_swap),
               testing::not_move_constructible*,
               testing::not_move_constructible*>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_iter_swap),
               testing::not_move_assignable*,
               testing::not_move_assignable*>);
}

/// @test @c std::iter_swap is not noexcept if type is not nothrow move assignable or move constructible.
CONSTEXPR_TEST(IterSwapTest, IterSwapIsCorrectlyNoExcept) {
  EXPECT_TRUE(noexcept(std::iter_swap(
      std::declval<testing::nothrow_move_construction_and_assignment*>(),
      std::declval<testing::nothrow_move_construction_and_assignment*>()
  )));

  EXPECT_FALSE(noexcept(std::iter_swap(
      std::declval<testing::throwable_move_construction*>(),
      std::declval<testing::throwable_move_construction*>()
  )));

  EXPECT_FALSE(noexcept(std::iter_swap(
      std::declval<testing::throwable_move_assignment*>(),
      std::declval<testing::throwable_move_assignment*>()
  )));
}

/// @test @c std::iter_swap exchanges the values of the objects pointer to
CONSTEXPR_TYPED_TEST(IterSwapTest, ValuesAreExchanged) {
  TypeParam first{0};
  TypeParam second{1};

  std::iter_swap(&first, &second);

  EXPECT_TRUE(first == TypeParam{1});
  EXPECT_TRUE(second == TypeParam{0});
}

/// @test ADL found swap function is used by @c std::iter_swap
CONSTEXPR_TEST(IterSwapTest, UsesADLFoundSwapFunction) {
  detail::type_with_swap_fn first{};
  detail::type_with_swap_fn second{};

  std::iter_swap(&first, &second);

  EXPECT_TRUE(first.swapped);
  EXPECT_TRUE(second.swapped);
}

/// @test Specializations of @c std::swap are found
CONSTEXPR_TEST(IterSwapTest, SpecializationsOfSwapAreFound) {
  iter_swap_test::type_with_swap_specialization first{};
  iter_swap_test::type_with_swap_specialization second{};

  std::iter_swap(&first, &second);

  EXPECT_TRUE(first.swapped);
  EXPECT_TRUE(second.swapped);
}

/// @test Specializations of @c std::iter_swap are found
CONSTEXPR_TEST(IterSwapTest, SpecializationsOfIterSwapAreFound) {
  iter_swap_test::type_with_iter_swap_specialization first{};
  iter_swap_test::type_with_iter_swap_specialization second{};

  std::iter_swap(&first, &second);

  EXPECT_TRUE(first.swapped);
  EXPECT_TRUE(second.swapped);
}

}  // namespace
