// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace swap_test {

struct type_with_swap_specialization {
  constexpr explicit type_with_swap_specialization() = default;

  bool swapped{false};
};
}  // namespace swap_test

namespace std {
template <>
constexpr void
swap(::swap_test::type_with_swap_specialization& lhs, ::swap_test::type_with_swap_specialization& rhs) noexcept {
  lhs.swapped = true;
  rhs.swapped = true;
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

using swap_types = ::testing::integral_types;

template <typename T>
class SwapTest : public testing::Test {};

TYPED_TEST_SUITE(SwapTest, swap_types, );

/// @test @c swap should always return void
TYPED_TEST(SwapTest, SwapHasCorrectReturnType) {
  testing::StaticAssertTypeEq<decltype(std::swap(std::declval<TypeParam&>(), std::declval<TypeParam&>())), void>();
}

/// @test The two arguments to @c swap are exchanged.
CONSTEXPR_TYPED_TEST(SwapTest, ValuesAreExchanged) {
  TypeParam first{0};
  TypeParam second{1};

  std::swap(first, second);

  EXPECT_TRUE(first == TypeParam{1});
  EXPECT_TRUE(second == TypeParam{0});
}

/// @test ADL found swap function not used when @c std::swap is explicitly invoked.
CONSTEXPR_TEST(SwapTest, NotForwardedToADLFunction) {
  detail::type_with_swap_fn first{};
  detail::type_with_swap_fn second{};

  // Explicitly invoking @c std::swap should use the standard library implementation.
  std::swap(first, second);

  EXPECT_FALSE(first.swapped);
  EXPECT_FALSE(second.swapped);

  // Using the two-step swap invokes ADL to find the swap friend function.
  using std::swap;
  swap(first, second);

  EXPECT_TRUE(first.swapped);
  EXPECT_TRUE(second.swapped);
}

/// @test Specializations of @c std::swap are found
CONSTEXPR_TEST(SwapTest, SpecializationsOfSwapAreFound) {
  swap_test::type_with_swap_specialization first{};
  swap_test::type_with_swap_specialization second{};

  std::swap(first, second);

  EXPECT_TRUE(first.swapped);
  EXPECT_TRUE(second.swapped);
}

struct track_move {
  constexpr track_move() = default;
  constexpr track_move(track_move const&) = default;
  constexpr track_move(track_move&&) noexcept
      : moved{true} {}
  constexpr auto operator=(track_move const&) -> track_move& { return *this; }
  constexpr auto operator=(track_move&&) noexcept -> track_move& {
    moved = true;
    return *this;
  }
  ~track_move() = default;

  bool moved{false};
};

/// @test The arguments to @c swap are moved if able.
CONSTEXPR_TEST(SwapTest, ValuesAreMoved) {
  track_move first{};
  track_move second{};

  std::swap(first, second);

  EXPECT_TRUE(first.moved);
  EXPECT_TRUE(second.moved);
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct track_copy {
  constexpr track_copy() = default;
  constexpr track_copy(track_copy const&)
      : copied{true} {}
  constexpr auto operator=(track_copy const&) -> track_copy& {
    copied = true;
    return *this;
  }

  bool copied{false};
};

/// @test The arguments to @c swap are copied if not movable.
CONSTEXPR_TEST(SwapTest, ValuesAreCopiedIfNotMovable) {
  track_copy first{};
  track_copy second{};

  std::swap(first, second);

  EXPECT_TRUE(first.copied);
  EXPECT_TRUE(second.copied);
}

/// @test @c swap is not noexcept if type is not nothrow move assignable or move constructible.
CONSTEXPR_TEST(SwapTest, SwapIsCorrectlyNoExcept) {
  EXPECT_TRUE(noexcept(std::swap(
      std::declval<testing::nothrow_move_construction_and_assignment&>(),
      std::declval<testing::nothrow_move_construction_and_assignment&>()
  )));

  EXPECT_FALSE(noexcept(std::swap(
      std::declval<testing::throwable_move_construction&>(),
      std::declval<testing::throwable_move_construction&>()
  )));

  EXPECT_FALSE(noexcept(std::swap(
      std::declval<testing::throwable_move_assignment&>(),
      std::declval<testing::throwable_move_assignment&>()
  )));
}

/// @test @c swap does not participate in overload resolution if the type is not move assignable or constructible.
TEST(SwapTest, SwapIsCorrectlyConstrained) {
  auto lifted_swap = [](auto&& lhs, auto&& rhs) -> decltype(std::swap(lhs, rhs)) { std::swap(lhs, rhs); };

  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_swap), int, int>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_swap),
               testing::not_move_constructible,
               testing::not_move_constructible>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_swap),
               testing::not_move_assignable,
               testing::not_move_assignable>);
}

using c_array_swap_types =
    arene::base::type_lists::concat_unique_t<::testing::signed_integer_types, ::testing::unsigned_integer_types>;

template <typename T>
class SwapCArrayTest : public testing::Test {};

TYPED_TEST_SUITE(SwapCArrayTest, c_array_swap_types, );

/// @test Given two operands of type @c T[N] , where @c arene::base::is_swappable_v<T> returns @c true , then
/// @c arene::base::swap(lhs,rhs) exchanges the values of every element in the input arrays as if by invoking
/// @c arene::base::swap on them.
CONSTEXPR_TYPED_TEST(SwapCArrayTest, TheValuesAreExchanged) {
  // Since you can't meaningfully copy a c-array, we initialize each twice with the same values.
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  constexpr TypeParam original_lhs[3] = {TypeParam{0}, TypeParam{1}, TypeParam{2}};
  TypeParam lhs[3] = {TypeParam{0}, TypeParam{1}, TypeParam{2}};

  constexpr TypeParam original_rhs[3] = {TypeParam{3}, TypeParam{4}, TypeParam{5}};
  TypeParam rhs[3] = {TypeParam{3}, TypeParam{4}, TypeParam{5}};
  // NOLINTEND(hicpp-avoid-c-arrays)

  std::swap(lhs, rhs);

  EXPECT_EQ(lhs[0], original_rhs[0]);
  EXPECT_EQ(lhs[1], original_rhs[1]);
  EXPECT_EQ(lhs[2], original_rhs[2]);

  EXPECT_EQ(rhs[0], original_lhs[0]);
  EXPECT_EQ(rhs[1], original_lhs[1]);
  EXPECT_EQ(rhs[2], original_lhs[2]);
}

/// @test @c swap for arrays is noexcept if the type's swap function is noexcept swappable.
CONSTEXPR_TEST(SwapCArrayTest, SwapIsCorrectlyNoExceptWithCustomSwapFunction) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  using nothrow = testing::nothrow_move_construction_and_assignment[2];
  using throwable1 = testing::throwable_move_construction[2];
  using throwable2 = testing::throwable_move_assignment[2];
  // NOLINTEND(hicpp-avoid-c-arrays)

  EXPECT_TRUE(noexcept(std::swap(std::declval<nothrow&>(), std::declval<nothrow&>())));
  EXPECT_FALSE(noexcept(std::swap(std::declval<throwable1&>(), std::declval<throwable1&>())));
  EXPECT_FALSE(noexcept(std::swap(std::declval<throwable2&>(), std::declval<throwable2&>())));
}

/// @test @c swap for arrays does not participate in overload resolution if the type is not move assignable or
/// constructible.
TEST(SwapCArrayTest, SwapIsCorrectlyConstrained) {
  auto lifted_swap = [](auto&& lhs, auto&& rhs) -> decltype(std::swap(lhs, rhs)) { std::swap(lhs, rhs); };

  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_swap), int[2], int[2]>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_swap), int[2], int[3]>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_swap),
               testing::not_move_constructible[2],
               testing::not_move_constructible[2]>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_swap),
               testing::not_move_assignable[2],
               testing::not_move_assignable[2]>);
  // NOLINTEND(hicpp-avoid-c-arrays)
}

}  // namespace
