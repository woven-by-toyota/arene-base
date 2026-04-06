// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file size_test.cpp
/// @brief Unit tests for arene/base/type_lists/contains.hpp
///

#include "arene/base/type_list/contains.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

namespace type_lists = ::arene::base::type_lists;

struct as_arene_type_list {
  template <typename... Ts>
  using type_list_t = ::arene::base::type_list<Ts...>;
};

struct as_gtest_type_list {
  template <typename... Ts>
  using type_list_t = ::testing::Types<Ts...>;
};

struct as_std_tuple {
  template <typename... Ts>
  using type_list_t = std::tuple<Ts...>;
};

using type_list_types = ::testing::Types<as_arene_type_list, as_gtest_type_list, as_std_tuple>;

template <typename T>
struct TypeListContainsTest : ::testing::Test {};

TYPED_TEST_SUITE(TypeListContainsTest, type_list_types, );

/// @test Given a type list type with no elements, `Tl<>`, the result of `arene::base::contains<Tl<>, T>()` and
/// `arene::base::contains_v<Tl<>, T>` is `false` for any `T`.
TYPED_TEST(TypeListContainsTest, ReturnsFalseForAllTypesIfTypeListIsEmpty) {
  using zero_size_tl = typename TypeParam::template type_list_t<>;
  STATIC_ASSERT_FALSE(type_lists::contains<zero_size_tl, void>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<zero_size_tl, void>);
  STATIC_ASSERT_FALSE(type_lists::contains<zero_size_tl, int>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<zero_size_tl, int>);
  STATIC_ASSERT_FALSE(type_lists::contains<zero_size_tl, int const>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<zero_size_tl, int const>);
}

/// @test Given a type list type, `Tl<Ts...>` and a type to search for, `T`, `arene::base::contains<Tl<Ts...>, T>()` and
/// `arene::base::contains_v<Tl<Ts...>, T>` will return `false` if there is no type in `Ts...` for which
/// `std::is_same<Ts, T>::value` is `true`.
TYPED_TEST(TypeListContainsTest, ReturnsFalseIfThereIsNotAnExactMatchOfTinTsIncludingCVRefQualification) {
  using does_not_contain_t = typename TypeParam::
      template type_list_t<double, int const volatile&, typename TypeParam::template type_list_t<int>>;
  STATIC_ASSERT_FALSE(type_lists::contains<does_not_contain_t, void>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<does_not_contain_t, void>);
  STATIC_ASSERT_FALSE(type_lists::contains<does_not_contain_t, int>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<does_not_contain_t, int>);
  STATIC_ASSERT_FALSE(type_lists::contains<does_not_contain_t, int&>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<does_not_contain_t, int&>);
  STATIC_ASSERT_FALSE(type_lists::contains<does_not_contain_t, int const&>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<does_not_contain_t, int const&>);
  STATIC_ASSERT_FALSE(type_lists::contains<does_not_contain_t, int volatile&>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<does_not_contain_t, int volatile&>);
  STATIC_ASSERT_FALSE(type_lists::contains<does_not_contain_t, int const volatile>());
  STATIC_ASSERT_FALSE(type_lists::contains_v<does_not_contain_t, int const volatile>);
}

/// @test Given a type list type, `Tl<Ts...>` and a type to search for, `T`, `arene::base::contains<Tl<Ts...>, T>()` and
/// `arene::base::contains_v<Tl<Ts...>, T>` will return `true` if there is a type in `Ts...` for which
/// `std::is_same<Ts, T>::value` is `true`.
TYPED_TEST(TypeListContainsTest, ReturnsTrueIfThereIsAnExactMatchOfTinTsIncludingCVRefQualification) {
  using contains_t = typename TypeParam::
      template type_list_t<double, int const volatile&, typename TypeParam::template type_list_t<int>>;
  STATIC_ASSERT_TRUE(type_lists::contains<contains_t, double>());
  STATIC_ASSERT_TRUE(type_lists::contains_v<contains_t, double>);
  STATIC_ASSERT_TRUE(type_lists::contains<contains_t, int const volatile&>());
  STATIC_ASSERT_TRUE(type_lists::contains_v<contains_t, int const volatile&>);
}

}  // namespace
