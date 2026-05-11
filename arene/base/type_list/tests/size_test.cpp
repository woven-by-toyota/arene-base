// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file size_test.cpp
/// @brief Unit tests for arene/base/type_lists/size.hpp
///

#include "arene/base/type_list/size.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

namespace type_lists = ::arene::base::type_lists;

template <template <typename...> class Tl, typename... Ts>
struct as_type_list {
  using type_list_t = Tl<Ts...>;
  static constexpr std::size_t expected_size = sizeof...(Ts);
};

template <template <typename...> class Tl, typename... Ts>
constexpr std::size_t as_type_list<Tl, Ts...>::expected_size;

struct as_arene_type_list {
  template <typename... Ts>
  using as = as_type_list<::arene::base::type_list, Ts...>;
};

struct as_gtest_type_list {
  template <typename... Ts>
  using as = as_type_list<::testing::Types, Ts...>;
};

struct as_std_tuple {
  template <typename... Ts>
  using as = as_type_list<std::tuple, Ts...>;
};

using type_list_types = ::testing::Types<as_arene_type_list, as_gtest_type_list, as_std_tuple>;

template <typename T>
struct TypeListSizeTest : ::testing::Test {};

TYPED_TEST_SUITE(TypeListSizeTest, type_list_types, );

/// @test Given a type list type with no elements, `Tl<>`, the result of `arene::base::size<Tl<>>()` and
/// `arene::base::size_v<Tl<>>` is `0U`.
TYPED_TEST(TypeListSizeTest, ATypeListWithNoElementsIsSize0) {
  using zero_size_tl = typename TypeParam::template as<>;
  STATIC_ASSERT_EQ(type_lists::size<typename zero_size_tl::type_list_t>(), zero_size_tl::expected_size);
  STATIC_ASSERT_EQ(type_lists::size_v<typename zero_size_tl::type_list_t>, zero_size_tl::expected_size);
}

/// @test Given a type list type, `Tl<Ts...>`, the result of `arene::base::size<Tl<Ts...>>()` and
/// `arene::base::size_v<Tl<Ts...>>` is equivalent to `sizeof...(Ts)`.
TYPED_TEST(TypeListSizeTest, ATypeListWithElementsIsSizeEquivalentToSizeofdddTs) {
  using size_one_tl = typename TypeParam::template as<int>;
  STATIC_ASSERT_EQ(type_lists::size<typename size_one_tl::type_list_t>(), size_one_tl::expected_size);
  STATIC_ASSERT_EQ(type_lists::size_v<typename size_one_tl::type_list_t>, size_one_tl::expected_size);
  using size_two_tl = typename TypeParam::template as<int, int>;
  STATIC_ASSERT_EQ(type_lists::size<typename size_two_tl::type_list_t>(), size_two_tl::expected_size);
  STATIC_ASSERT_EQ(type_lists::size_v<typename size_two_tl::type_list_t>, size_two_tl::expected_size);
  using size_three_tl = typename TypeParam::template as<int, int, int>;
  STATIC_ASSERT_EQ(type_lists::size<typename size_three_tl::type_list_t>(), size_three_tl::expected_size);
  STATIC_ASSERT_EQ(type_lists::size_v<typename size_three_tl::type_list_t>, size_three_tl::expected_size);
}

template <typename T>
struct TypeListEmptyTest : ::testing::Test {};

TYPED_TEST_SUITE(TypeListEmptyTest, type_list_types, );

/// @test Given a type list type with no elements, `Tl<>`, the result of `arene::base::empty<Tl<>>()` and
/// `arene::base::empty_v<Tl<>>` is `true`.
TYPED_TEST(TypeListEmptyTest, ASizeZeroTypeListIsEmpty) {
  using zero_size_tl = typename TypeParam::template as<>;
  STATIC_ASSERT_TRUE(type_lists::empty<typename zero_size_tl::type_list_t>());
  STATIC_ASSERT_TRUE(type_lists::empty_v<typename zero_size_tl::type_list_t>);
}

/// @test Given a type list type, `Tl<Ts...>`, the result of `arene::base::empty<Tl<Ts...>>()` and
/// `arene::base::empty_v<Tl<Ts...>>` is `false`.
TYPED_TEST(TypeListSizeTest, ASizeNonZeroTypeListIsNotEmpty) {
  using size_one_tl = typename TypeParam::template as<int>;
  STATIC_ASSERT_FALSE(type_lists::empty<typename size_one_tl::type_list_t>());
  STATIC_ASSERT_FALSE(type_lists::empty_v<typename size_one_tl::type_list_t>);
  using size_two_tl = typename TypeParam::template as<int, int>;
  STATIC_ASSERT_FALSE(type_lists::empty<typename size_two_tl::type_list_t>());
  STATIC_ASSERT_FALSE(type_lists::empty_v<typename size_two_tl::type_list_t>);
  using size_three_tl = typename TypeParam::template as<int, int, int>;
  STATIC_ASSERT_FALSE(type_lists::empty<typename size_three_tl::type_list_t>());
  STATIC_ASSERT_FALSE(type_lists::empty_v<typename size_three_tl::type_list_t>);
}

}  // namespace
