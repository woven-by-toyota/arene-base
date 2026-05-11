// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

using arene::base::type_lists::concat_unique_t;
using arene::base::type_lists::remove_duplicates_t;

/// @brief An arbitrary size for an array type
constexpr std::size_t array_size = 3;

using non_function_types = concat_unique_t<::testing::scalar_types, ::testing::union_types, ::testing::class_types>;

template <typename T>
class DecayObjectTest : public testing::Test {};

TYPED_TEST_SUITE(DecayObjectTest, non_function_types, );

/// @test @c decay_t on non-array, non-function objects evaluates to a reference and cv-erased type
TYPED_TEST(DecayObjectTest, ObjectTypes) {
  static_assert(std::is_same_v<std::decay_t<TypeParam>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam const>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam volatile>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam const volatile>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam const&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam volatile&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam const volatile&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam&&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam const&&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam volatile&&>, TypeParam>, "Decay Object");
  static_assert(std::is_same_v<std::decay_t<TypeParam const volatile&&>, TypeParam>, "Decay Object");
}

template <typename T>
class DecayArrayTest : public testing::Test {};

TYPED_TEST_SUITE(DecayArrayTest, remove_duplicates_t<::testing::extend_with_cv_qualifiers_t<non_function_types>>, );

/// @test @c decay_t on an array type evaluates to a reference-erased pointer type
TYPED_TEST(DecayArrayTest, ArrayTypes) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert(std::is_same_v<std::decay_t<TypeParam[]>, TypeParam*>, "Decay Array");
  static_assert(std::is_same_v<std::decay_t<TypeParam(&)[]>, TypeParam*>, "Decay Array");
  static_assert(std::is_same_v<std::decay_t<TypeParam(&&)[]>, TypeParam*>, "Decay Array");
  static_assert(std::is_same_v<std::decay_t<TypeParam[array_size]>, TypeParam*>, "Decay Array");
  static_assert(std::is_same_v<std::decay_t<TypeParam(&)[array_size]>, TypeParam*>, "Decay Array");
  static_assert(std::is_same_v<std::decay_t<TypeParam(&&)[array_size]>, TypeParam*>, "Decay Array");
  // NOLINTEND(hicpp-avoid-c-arrays)
}

template <typename T>
class DecayFunctionTest : public testing::Test {};

TYPED_TEST_SUITE(
    DecayFunctionTest,
    remove_duplicates_t<::testing::extend_with_cv_qualifiers_t<::testing::function_types>>,
);

/// @test @c decay_t on a function type evaluates to the pointer type
TYPED_TEST(DecayFunctionTest, FunctionTypes) {
  static_assert(std::is_same_v<std::decay_t<TypeParam>, TypeParam*>, "Decay Function");
  static_assert(std::is_same_v<std::decay_t<TypeParam&>, TypeParam*>, "Decay Function");
  static_assert(std::is_same_v<std::decay_t<TypeParam&&>, TypeParam*>, "Decay Function");
}

}  // namespace
