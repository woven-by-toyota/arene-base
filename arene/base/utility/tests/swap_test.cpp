// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/swap.hpp"

#include <gmock/gmock.h>  // IWYU pragma: keep
#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_swappable.hpp"

namespace arene {
namespace base {
// this validates we can place hidden friend overloads of swap in the arene::base namespace without conflicts.
struct swappable_test_swappable {
  friend constexpr void swap(swappable_test_swappable& lhs, swappable_test_swappable& rhs) noexcept {
    int const tmp = lhs.value;
    lhs.value = rhs.value;
    rhs.value = tmp;
  }

  int value;
  friend constexpr auto operator==(swappable_test_swappable const& lhs, swappable_test_swappable const& rhs) -> bool {
    return lhs.value == rhs.value;
  }
};

}  // namespace base
}  // namespace arene

namespace {
using ::arene::base::is_nothrow_swappable_v;
using ::arene::base::is_swappable_v;
using ::arene::base::substitution_succeeds;
using ::arene::base::type_list;
using ::arene::base::type_lists::apply_all_t;
using ::arene::base::type_lists::concat_t;

/// Helper for casting a type_list to testing::Types
template <typename Tl>
using as_testing_types = apply_all_t<Tl, ::testing::Types>;

/// used to select which operand is returned from @c swap_in_constexpr
enum class return_opr { lhs, rhs };

/// A simple helper that performs swap in a constexpr context
/// @param lhs the left hand operand
/// @param rhs the right hand operand
/// @param ret controls which operand is returned
/// @return If @c ret::lhs , then @c lhs is returned, else @c rhs is returned, post swap.
template <typename T>
constexpr auto swap_in_constexpr(T lhs, T rhs, return_opr ret) {
  ::arene::base::swap(lhs, rhs);
  return ret == return_opr::lhs ? std::move(lhs) : std::move(rhs);
}

/// our base fixture
template <typename T>
class SwapTest : public ::testing::Test {};

///
/// @brief A CRTP base class for injecting a value and equality operator into a swappable type.
///
template <typename T>
class swappable_value {
 public:
  friend constexpr auto operator==(T const& lhs, T const& rhs) -> bool {
    return static_cast<swappable_value const&>(lhs).value_ == static_cast<swappable_value const&>(rhs).value_;
  }
  ~swappable_value() = default;
  constexpr explicit swappable_value(int value)
      : value_(value) {}
  constexpr swappable_value(swappable_value const&) noexcept = default;
  constexpr swappable_value(swappable_value&&) noexcept = default;
  constexpr auto operator=(swappable_value const&) noexcept -> swappable_value& = default;
  constexpr auto operator=(swappable_value&&) noexcept -> swappable_value& = default;

 protected:
  int value_;
};

class nothrow_default_swappable : public swappable_value<nothrow_default_swappable> {
 public:
  using swappable_value<nothrow_default_swappable>::swappable_value;
};

class throwing_move_ctor_default_swappable : public swappable_value<throwing_move_ctor_default_swappable> {
 public:
  using swappable_value<throwing_move_ctor_default_swappable>::swappable_value;
  ~throwing_move_ctor_default_swappable() = default;
  constexpr throwing_move_ctor_default_swappable(throwing_move_ctor_default_swappable const&) = default;
  constexpr auto operator=(throwing_move_ctor_default_swappable const&)
      -> throwing_move_ctor_default_swappable& = default;

  constexpr throwing_move_ctor_default_swappable(throwing_move_ctor_default_swappable&&) noexcept(false) = default;
  constexpr auto operator=(throwing_move_ctor_default_swappable&&) noexcept
      -> throwing_move_ctor_default_swappable& = default;
};

class throwing_move_assign_default_swappable : public swappable_value<throwing_move_assign_default_swappable> {
 public:
  using swappable_value<throwing_move_assign_default_swappable>::swappable_value;
  ~throwing_move_assign_default_swappable() = default;
  constexpr throwing_move_assign_default_swappable(throwing_move_assign_default_swappable const&) = default;
  constexpr auto operator=(throwing_move_assign_default_swappable const&)
      -> throwing_move_assign_default_swappable& = default;

  constexpr throwing_move_assign_default_swappable(throwing_move_assign_default_swappable&&) noexcept = default;
  constexpr auto operator=(throwing_move_assign_default_swappable&&) noexcept(false)
      -> throwing_move_assign_default_swappable& = default;
};

// A type that's swappable with an unwrapped T but not with itself
template <typename T>
struct swappable_with_parameter {
  T value;

  friend constexpr auto swap(swappable_with_parameter& left, T& right) -> void { arene::base::swap(left.value, right); }

  friend constexpr auto swap(T& left, swappable_with_parameter& right) -> void { arene::base::swap(left, right.value); }

  friend constexpr auto operator==(swappable_with_parameter const& left, swappable_with_parameter const& right)
      -> bool {
    return left.value == right.value;
  }

  friend constexpr auto operator==(swappable_with_parameter const& left, T const& right) -> bool {
    return left.value == right;
  }

  friend constexpr auto operator==(T const& left, swappable_with_parameter const& right) -> bool {
    return left == right.value;
  }
};

using default_swappable_types = type_list<
    int,
    nothrow_default_swappable,
    throwing_move_assign_default_swappable,
    throwing_move_ctor_default_swappable,
    ::arene::base::swappable_test_swappable>;

template <typename T>
using SwapDefaultSwappableTest = SwapTest<T>;

TYPED_TEST_SUITE(SwapDefaultSwappableTest, as_testing_types<default_swappable_types>, );
/// @test Given two operands of type @c T& , where @c arene::base::is_swappable_v<T> returns @c true and there is no
/// customization for @c T , then @c arene::base::swap(lhs,rhs) is equivalent to std::swap(lhs,rhs)
TYPED_TEST(SwapDefaultSwappableTest, TheValuesAreExchanged) {
  constexpr auto original_lhs = TypeParam{1};
  constexpr auto original_rhs = TypeParam{2};
  STATIC_ASSERT_EQ(swap_in_constexpr(original_lhs, original_rhs, return_opr::lhs), original_rhs);
  STATIC_ASSERT_EQ(swap_in_constexpr(original_lhs, original_rhs, return_opr::rhs), original_lhs);
}

class nothrow_adl_swappable : public swappable_value<nothrow_adl_swappable> {
 public:
  using swappable_value<nothrow_adl_swappable>::swappable_value;

  bool swap_called = false;

  friend constexpr void swap(nothrow_adl_swappable& lhs, nothrow_adl_swappable& rhs) noexcept {
    ::arene::base::swap(lhs.value_, rhs.value_);
    lhs.swap_called = true;
    rhs.swap_called = true;
  }
};

class throwing_adl_swappable : public swappable_value<throwing_adl_swappable> {
 public:
  using swappable_value<throwing_adl_swappable>::swappable_value;

  bool swap_called = false;

  friend constexpr void swap(throwing_adl_swappable& lhs, throwing_adl_swappable& rhs) noexcept {
    ::arene::base::swap(lhs.value_, rhs.value_);
    lhs.swap_called = true;
    rhs.swap_called = true;
  }
};

using adl_swappable_types = type_list<throwing_adl_swappable, nothrow_adl_swappable>;

template <typename T>
class SwapADLSwappableTest : public SwapTest<T> {};

TYPED_TEST_SUITE(SwapADLSwappableTest, as_testing_types<adl_swappable_types>, );
/// @test Given two operands of type @c T& , where @c arene::base::is_swappable_v<T> returns @c true and there is a
/// definition of @c swap(T&,T&) discoverable via ADL, @c arene::base::swap(lhs,rhs) invokes the ADL discovered
/// customization point.
TYPED_TEST(SwapADLSwappableTest, InvokesADLDiscoveredSwap) {
  constexpr auto original_lhs = TypeParam{1};
  constexpr auto original_rhs = TypeParam{2};

  STATIC_ASSERT_TRUE(swap_in_constexpr(original_lhs, original_rhs, return_opr::lhs).swap_called);
  STATIC_ASSERT_TRUE(swap_in_constexpr(original_lhs, original_rhs, return_opr::rhs).swap_called);
}
/// @test Given two operands of type @c T& , where @c arene::base::is_swappable_v<T> returns @c true and there is a
/// definition of @c swap(T&,T&) discoverable via ADL, @c arene::base::swap(lhs,rhs) is equivalent to invoking the
/// discovered ADL customization point on the input operands.
TYPED_TEST(SwapADLSwappableTest, TheValuesAreExchanged) {
  constexpr auto original_lhs = TypeParam{1};
  constexpr auto original_rhs = TypeParam{2};

  STATIC_ASSERT_EQ(swap_in_constexpr(original_lhs, original_rhs, return_opr::lhs), original_rhs);
  STATIC_ASSERT_EQ(swap_in_constexpr(original_lhs, original_rhs, return_opr::rhs), original_lhs);
}

/// @test Two operands of different types which are swappable with each other can be swapped by @c arene::base::swap
TYPED_TEST(SwapADLSwappableTest, HeterogeneousAdlSwap) {
  constexpr TypeParam original_lhs{1};
  constexpr swappable_with_parameter<TypeParam> original_rhs{TypeParam{2}};

  TypeParam lhs{original_lhs};
  swappable_with_parameter<TypeParam> rhs{original_rhs};

  arene::base::swap(lhs, rhs);
  EXPECT_EQ(original_lhs, rhs.value);
  EXPECT_EQ(original_rhs.value, lhs);

  arene::base::swap(rhs, lhs);
  EXPECT_EQ(original_lhs, lhs);
  EXPECT_EQ(original_rhs.value, rhs.value);
}

using swappable_types = concat_t<default_swappable_types, adl_swappable_types>;

template <typename T>
class SwapNoexcepTest : public SwapTest<T> {};

TYPED_TEST_SUITE(SwapNoexcepTest, as_testing_types<swappable_types>, );
/// @test Given two operands of type @c T& , @c noexcept(arene::base::swap(lhs,rhs)) matches
/// @c arene::base::is_nothrow_swappable_v<T> .
TYPED_TEST(SwapNoexcepTest, NoexceptMatchesIsNothrowSwappable) {
  STATIC_ASSERT_EQ(noexcept(::arene::base::swap(std::declval<TypeParam&>(), std::declval<TypeParam&>())), is_nothrow_swappable_v<TypeParam>);
}

struct not_move_constructable {
  not_move_constructable() = default;
  ~not_move_constructable() = default;

  not_move_constructable(not_move_constructable const&) = delete;
  auto operator=(not_move_constructable const&) -> not_move_constructable& = delete;

  not_move_constructable(not_move_constructable&&) = delete;
  auto operator=(not_move_constructable&&) -> not_move_constructable& = default;
};

struct not_move_assignable {
  not_move_assignable() = default;
  ~not_move_assignable() = default;

  not_move_assignable(not_move_assignable const&) = delete;
  auto operator=(not_move_assignable const&) -> not_move_assignable& = delete;

  not_move_assignable(not_move_assignable&&) = default;
  auto operator=(not_move_assignable&&) -> not_move_assignable& = delete;
};

using non_swappable_types = type_list<not_move_assignable, not_move_constructable>;

using swappable_and_non_swappable_types = concat_t<swappable_types, non_swappable_types>;

template <typename T>
using use_swap = decltype(::arene::base::swap(std::declval<T&>(), std::declval<T&>()));

template <typename T>
constexpr bool swap_exists = substitution_succeeds<use_swap, T>;

template <typename T>
class SwapConstraintTest : public SwapTest<T> {};

TYPED_TEST_SUITE(SwapConstraintTest, as_testing_types<swappable_types>, );
/// @test Given two operands of type @c T& , @c arene::base::swap(lhs,rhs) participates in overload resolution only if
/// @c arene::base::is_swappable_v<T> returns @c true .
TYPED_TEST(SwapConstraintTest, ParticipatesInOverloadResolutionOnlyForTypesWhichIsSwappableIsTrue) {
  STATIC_ASSERT_EQ(swap_exists<TypeParam>, is_swappable_v<TypeParam>);
}

// NOLINTBEGIN(hicpp-avoid-c-arrays) explicitly testing c-array behavior

template <typename T>
class SwapCArrayTest : public SwapTest<T> {};

TYPED_TEST_SUITE(SwapCArrayTest, as_testing_types<swappable_types>, );
/// @test Given two operands of type @c T[N] , where @c arene::base::is_swappable_v<T> returns @c true , then
/// @c arene::base::swap(lhs,rhs) exchanges the values of every element in the input arrays as if by invoking
/// @c arene::base::swap on them.
TYPED_TEST(SwapCArrayTest, TheValuesAreExchanged) {
  // Since you can't meaningfully copy a c-array, we initialize each twice with the same values.
  constexpr TypeParam original_lhs[3] = {TypeParam{1}, TypeParam{2}, TypeParam{3}};
  TypeParam lhs[3] = {TypeParam{1}, TypeParam{2}, TypeParam{3}};

  constexpr TypeParam original_rhs[3] = {TypeParam{4}, TypeParam{5}, TypeParam{6}};
  TypeParam rhs[3] = {TypeParam{4}, TypeParam{5}, TypeParam{6}};

  ::arene::base::swap(lhs, rhs);

  EXPECT_THAT(lhs, ::testing::ElementsAreArray(original_rhs));
  EXPECT_THAT(rhs, ::testing::ElementsAreArray(original_lhs));
}

/// @test Two C-arrays with different element types can be swapped if the element types can
TYPED_TEST(SwapCArrayTest, CArraysWithDifferentTypesCanBeSwapped) {
  constexpr std::size_t extent{3U};

  constexpr TypeParam original_lhs[extent] = {TypeParam{1}, TypeParam{2}, TypeParam{3}};
  TypeParam lhs[extent] = {TypeParam{1}, TypeParam{2}, TypeParam{3}};

  constexpr swappable_with_parameter<TypeParam> original_rhs[extent] = {{TypeParam{4}}, {TypeParam{5}}, {TypeParam{6}}};
  swappable_with_parameter<TypeParam> rhs[extent] = {{TypeParam{4}}, {TypeParam{5}}, {TypeParam{6}}};

  ::arene::base::swap(lhs, rhs);

  EXPECT_THAT(lhs, ::testing::ElementsAreArray(original_rhs));
  EXPECT_THAT(rhs, ::testing::ElementsAreArray(original_lhs));

  ::arene::base::swap(rhs, lhs);

  EXPECT_THAT(lhs, ::testing::ElementsAreArray(original_lhs));
  EXPECT_THAT(rhs, ::testing::ElementsAreArray(original_rhs));
}

// NOLINTEND(hicpp-avoid-c-arrays) explicitly testing c-array behavior

}  // namespace
