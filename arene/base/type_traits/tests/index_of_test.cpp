// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/index_of.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::index_of;
using ::arene::base::index_of_v;
using ::arene::base::last_index_of;
using ::arene::base::last_index_of_v;
using ::arene::base::substitution_succeeds;
using ::arene::base::index_of_detail::index_of_first_true;
using ::arene::base::index_of_detail::index_of_last_true;

struct user_defined;

/// @test `arene::base::index_of_detail::index_of_first_true` returns the number of elements in the supplied list if
/// they are all `false`.
TEST(IndexOfFirstTrue, ReturnsSizeOfSetIAllAreFalse) {
  STATIC_ASSERT_EQ(index_of_first_true({}), 0);
  STATIC_ASSERT_EQ(index_of_first_true({false}), 1);
  STATIC_ASSERT_EQ(index_of_first_true({false, false}), 2);
}

/// @test `arene::base::index_of_detail::index_of_first_true` returns the index of the first `true` element in the
/// supplied list if there is at least one `true` element
TEST(IndexOfFirstTrue, ReturnsIndexOfFirstTrue) {
  STATIC_ASSERT_EQ(index_of_first_true({true}), 0);
  STATIC_ASSERT_EQ(index_of_first_true({false, true}), 1);
  STATIC_ASSERT_EQ(index_of_first_true({false, true, true}), 1);
  STATIC_ASSERT_EQ(index_of_first_true({false, false, true, false, false, true, false}), 2);
}

/// @test `index_of` has a static `constexpr` member `value` which is the index of the first occurrence of the first
/// template parameter in the list of types formed from the subsequent template parameters
TEST(IndexOf, HasMemberValueWithIndexOfFirstMatch) {
  STATIC_ASSERT_EQ((index_of<int, int>::value), 0);
  STATIC_ASSERT_EQ((index_of<int, char, int, float, int>::value), 1);
  STATIC_ASSERT_EQ((index_of<double, char, int, float, double, int, double, char>::value), 3);
}

template <typename T, typename... Ts>
using use_index_of = decltype(index_of<T, Ts...>::value);

/// @test `index_of` does not have a static `constexpr` member `value` if the first template parameter is not present in
/// the list of types formed from the remaining template parameters
TEST(IndexOf, HasNoValueMemberIfTNotInTs) {
  STATIC_ASSERT_FALSE(substitution_succeeds<use_index_of, int, char>);
  STATIC_ASSERT_FALSE(substitution_succeeds<use_index_of, user_defined, char, int, float>);
}

/// @test `index_of_v` evaluates to the index of the first occurrence of the first template parameter in the list of
/// types formed from the subsequent template parameters
TEST(IndexOfV, ReturnsIndexOfFirstMatch) {
  STATIC_ASSERT_EQ((index_of_v<int, int>), 0);
  STATIC_ASSERT_EQ((index_of_v<int, char, int, float, int, user_defined>), 1);
  STATIC_ASSERT_EQ((index_of_v<user_defined, char, int, float, int, user_defined>), 4);
}

/// @test `arene::base::index_of_detail::index_of_last_true` returns the number of elements in the supplied list if
/// they are all `false`.
TEST(IndexOfLastTrue, ReturnsSizeOfSetIAllAreFalse) {
  STATIC_ASSERT_EQ(index_of_last_true({}), 0);
  STATIC_ASSERT_EQ(index_of_last_true({false}), 1);
  STATIC_ASSERT_EQ(index_of_last_true({false, false}), 2);
}

/// @test `arene::base::index_of_detail::index_of_last_true` returns the index of the last `true` element in the
/// supplied list if there is at least one `true` element
TEST(IndexOfLastTrue, ReturnsIndexOfFirstTrue) {
  STATIC_ASSERT_EQ(index_of_last_true({true}), 0);
  STATIC_ASSERT_EQ(index_of_last_true({false, true}), 1);
  STATIC_ASSERT_EQ(index_of_last_true({false, true, true}), 2);
  STATIC_ASSERT_EQ(index_of_last_true({false, false, true, false, false, true, false}), 5);
}

/// @test `last_index_of` has a static `constexpr` member `value` which is the index of the last occurrence of the first
/// template parameter in the list of types formed from the subsequent template parameters
TEST(LastIndexOf, HasMemberValueWithIndexOfFirstMatch) {
  STATIC_ASSERT_EQ((last_index_of<int, int>::value), 0);
  STATIC_ASSERT_EQ((last_index_of<int, char, int, float, int>::value), 3);
  STATIC_ASSERT_EQ((last_index_of<double, char, int, float, double, int, double, char>::value), 5);
}

template <typename T, typename... Ts>
using use_last_index_of = decltype(last_index_of<T, Ts...>::value);

/// @test `last_index_of` does not have a static `constexpr` member `value` if the first template parameter is not
/// present in the list of types formed from the remaining template parameters
TEST(LastIndexOf, HasNoValueMemberIfTNotInTs) {
  STATIC_ASSERT_FALSE(substitution_succeeds<use_last_index_of, int, char>);
  STATIC_ASSERT_FALSE(substitution_succeeds<use_last_index_of, user_defined, char, int, float>);
}

/// @test `last_index_of_v` evaluates to the index of the last occurrence of the first template parameter in the list of
/// types formed from the subsequent template parameters
TEST(LastIndexOfV, ReturnsIndexOfFirstMatch) {
  STATIC_ASSERT_EQ((last_index_of_v<int, int>), 0);
  STATIC_ASSERT_EQ((last_index_of_v<int, char, int, float, int, user_defined>), 3);
  STATIC_ASSERT_EQ((last_index_of_v<user_defined, char, int, float, int, user_defined>), 4);
}

}  // namespace
