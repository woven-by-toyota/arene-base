// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

using arene::base::inline_vector;

using MyTypes = ::testing::Types<   //
    inline_vector<double, 1>,       //
    inline_vector<int, 10>,         //
    inline_vector<int, 42>,         //
    inline_vector<int, 100'000>,    //
    inline_vector<int, 1'000'000>,  //
    inline_vector<long, 50'000>     // NOLINT(google-runtime-int)
    >;

template <class T>
struct InlineVectorThrow : ::testing::Test {};

TYPED_TEST_SUITE(InlineVectorThrow, MyTypes, );

/// @test An @c inline_vector throws if the argument of @c at is equal to the @c capacity
TYPED_TEST(InlineVectorThrow, AtThrowsLvalueReference) {
  auto vec1 = TypeParam(TypeParam::capacity());
  std::ignore = vec1.at(0U);
  ASSERT_THROW(vec1.at(TypeParam::capacity()), std::out_of_range);

  auto const vec2 = TypeParam(TypeParam::capacity());
  std::ignore = vec2.at(0U);
  ASSERT_THROW(vec2.at(TypeParam::capacity()), std::out_of_range);
}

/// @test An @c inline_vector throws if the argument of @c at is equal to the @c capacity
TYPED_TEST(InlineVectorThrow, AtThrowsRvalueReference) {
  // NOLINTBEGIN(bugprone-use-after-move)
  auto vec3 = TypeParam(TypeParam::capacity());
  std::ignore = std::move(vec3).at(0U);
  ASSERT_THROW(std::move(vec3).at(TypeParam::capacity()), std::out_of_range);

  auto const vec4 = TypeParam(TypeParam::capacity());
  std::ignore = std::move(vec4).at(0U);
  ASSERT_THROW(std::move(vec4).at(TypeParam::capacity()), std::out_of_range);
  // NOLINTEND(bugprone-use-after-move)
}

}  // namespace
