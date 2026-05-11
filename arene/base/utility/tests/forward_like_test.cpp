// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/forward_like.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/flat_map.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {
using ::arene::base::forward_like;
using ::arene::base::type_list;
using ::arene::base::type_lists::apply_all_t;
using ::arene::base::type_lists::concat_t;
using ::arene::base::type_lists::flat_map_t;

template <typename T>
using add_const = type_list<std::add_const_t<T>>;
template <typename T>
using add_lvalue_ref = type_list<std::add_lvalue_reference_t<T>>;
template <typename T>
using add_rvalue_ref = type_list<std::add_rvalue_reference_t<T>>;

template <typename Tl>
using as_types = apply_all_t<Tl, ::testing::Types>;

struct user_type {};

using types = type_list<int, float, user_type>;
using const_types = flat_map_t<types, add_const>;
using lvalue_reference_types = flat_map_t<types, add_lvalue_ref>;
using const_lvalue_reference_types = flat_map_t<const_types, add_lvalue_ref>;
using rvalue_reference_types = flat_map_t<types, add_rvalue_ref>;
using const_rvalue_reference_types = flat_map_t<const_types, add_rvalue_ref>;
using all_to_types = concat_t<
    lvalue_reference_types,
    rvalue_reference_types,
    const_lvalue_reference_types,
    const_rvalue_reference_types>;

template <typename From, typename To>
using forward_like_type = decltype(forward_like<From>(std::declval<To>()));

template <typename T>
constexpr bool is_const_reference = std::is_const<std::remove_reference_t<T>>::value;

template <typename T>
struct ForwardLikeTest : public ::testing::Test {};

TYPED_TEST_SUITE(ForwardLikeTest, as_types<all_to_types>, );
/// @test arene::base::forward_like<From,To>(value) 's return type is @c const qualified if either @c From or @c To is
/// @c const qualified.
TYPED_TEST(ForwardLikeTest, ConstnessIsTheMergeOfFromAndToConstness) {
  // if From is non-const, we're const if To is const
  STATIC_ASSERT_EQ((is_const_reference<forward_like_type<int&, TypeParam>>), is_const_reference<TypeParam>);
  // if From is const, we're always const
  STATIC_ASSERT_TRUE((is_const_reference<forward_like_type<int const&, TypeParam>>));
}
/// @test arene::base::forward_like<From,To>(value) 's return type is an lvalue reference if @c From is an
/// lvalue-reference .
TYPED_TEST(ForwardLikeTest, IfFromIsLValueReturnTypeIsLvalueReference) {
  STATIC_ASSERT_TRUE((std::is_lvalue_reference<forward_like_type<int&, TypeParam>>::value));
  STATIC_ASSERT_TRUE((std::is_lvalue_reference<forward_like_type<int const&, TypeParam>>::value));
}
/// @test arene::base::forward_like<From,To>(value) 's return type is an rvalue reference if @c From is an
/// lvalue-reference .
TYPED_TEST(ForwardLikeTest, IfFromIsRValueReturnTypeIsRvalueReference) {
  STATIC_ASSERT_TRUE((std::is_rvalue_reference<forward_like_type<int&&, TypeParam>>::value));
  STATIC_ASSERT_TRUE((std::is_rvalue_reference<forward_like_type<int const&&, TypeParam>>::value));
}

template <typename Lhs, typename Rhs>
auto compare_addresses(Lhs&& lhs, Rhs&& rhs) -> bool {
  return std::addressof(lhs) == std::addressof(rhs);
}

/// @test arene::base::forward_like<From,To>(value) returns a reference to @c value
TYPED_TEST(ForwardLikeTest, ReturnsReferenceToInputValue) {
  // NOLINTNEXTLINE(misc-const-correctness) explicitly testing lvalue reference behavior.
  using raw_type = std::remove_reference_t<TypeParam>;
  raw_type value{};
  EXPECT_TRUE(compare_addresses(forward_like<raw_type&>(value), value));
  EXPECT_TRUE(compare_addresses(forward_like<raw_type&&>(value), value));
  EXPECT_TRUE(compare_addresses(forward_like<raw_type const&>(value), value));
  EXPECT_TRUE(compare_addresses(forward_like<raw_type const&&>(value), value));
}

/// @test Given a type `T` which is qualified with `volatile` and a value of type `V` which is not qualified with
/// `volatile`, then the type of the return value of `forward_like<T>(value)` is not qualified with `volatile`.
TEST(ForwardLikeTest, DoesNotPropagateVolatile) {
  static_assert(
      std::is_same<  //
          int const&,
          forward_like_type<int const volatile&, int>>::value,
      "must not propagate volatile from the source"
  );

  static_assert(
      std::is_same<  //
          int const&,
          forward_like_type<int volatile&, int const>>::value,
      "must not propagate volatile from the souce"
  );
}

/// @test Given a type `T` which is not qualified with `volatile` and a value of type `V` which is qualified with
/// `volatile`, then the type of the return value of `forward_like<T>(value)` is qualified with `volatile`.
TEST(ForwardLikeTest, DoesNotRemoveVolatile) {
  static_assert(
      std::is_same<  //
          int const volatile&,
          forward_like_type<int const&, int volatile>>::value,
      "must not remove volatile from the dest"
  );
}

/// @test forward_like returns an rvalue reference if the type is not a reference
TEST(ForwardLikeTest, ReturnsRvalueReference) {
  static_assert(
      std::is_same<  //
          int&&,
          forward_like_type<int, int&>>::value,
      "must be an rvalue reference"
  );
}

}  // namespace
