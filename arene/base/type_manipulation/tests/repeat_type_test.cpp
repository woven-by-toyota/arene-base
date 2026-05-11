// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/repeat_type.hpp"

#include <cstdint>
#include <tuple>
#include <type_traits>

#include <gtest/gtest.h>

namespace {

using arene::base::repeat_type;
using arene::base::repeat_type_ex;

/// @test `repeat_type<0,Template,Type>` ignores the type parameter when the count is zero, and returns `Template<>`
TEST(RepeatTypeTest, Repeat0Times) {
  using t = typename repeat_type<0UL, std::tuple, std::int8_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 0UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<>>::value));
}

/// @test `repeat_type<1,Template,Type>` with a count of 1 returns `Template<Type>`
TEST(RepeatTypeTest, Repeat1Time) {
  using t = typename repeat_type<1UL, std::tuple, std::int8_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 1UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<std::int8_t>>::value));
}

/// @test `repeat_type<2,Template,Type>` with a count of 2 returns `Template<Type,Type>`
TEST(RepeatTypeTest, Repeat2Times) {
  using t = typename repeat_type<2UL, std::tuple, std::int16_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 2UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<std::int16_t, std::int16_t>>::value));
}

/// @test `repeat_type<3,Template,Type>` with a count of 3 returns `Template<Type,Type,Type>`
TEST(RepeatTypeTest, Repeate3Times) {
  using t = typename repeat_type<3UL, std::tuple, std::int32_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 3UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<std::int32_t, std::int32_t, std::int32_t>>::value));
}

/// @test `repeat_type<4,Template,Type>` with a count of 4 returns `Template<Type,Type,Type,Type>`
TEST(RepeatTypeTest, Repeat4Times) {
  using t = typename repeat_type<4UL, std::tuple, std::int64_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 4UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<std::int64_t, std::int64_t, std::int64_t, std::int64_t>>::value));
}

/// @test `repeat_type<0,Template,Type1,Type2>` ignores the type parameters when the count is zero, and returns
/// `Template<>`
TEST(RepeatTypeExTest, Repeat0Times) {
  using t = typename repeat_type_ex<0UL, std::tuple, float, std::int8_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 1UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<float>>::value));
}

/// @test `repeat_type<1,Template,Type1,Type2>` with a count of 1 returns `Template<Type1,Type2>`
TEST(RepeatTypeExTest, Repeat1Time) {
  using t = typename repeat_type_ex<1UL, std::tuple, double, std::int8_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 2UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<double, std::int8_t>>::value));
}

/// @test `repeat_type<2,Template,Type1,Type2>` with a count of 2 returns `Template<Type1,Type2,Type2>`
TEST(RepeatTypeExTest, Repeat2Times) {
  using t = typename repeat_type_ex<2UL, std::tuple, float, std::int16_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 3UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<float, std::int16_t, std::int16_t>>::value));
}

/// @test `repeat_type<3,Template,Type1,Type2>` with a count of 3 returns `Template<Type1,Type2,Type2,Type2>`
TEST(RepeatTypeExTest, Repeate3Times) {
  using t = typename repeat_type_ex<3UL, std::tuple, double, std::int32_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 4UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<double, std::int32_t, std::int32_t, std::int32_t>>::value));
}

/// @test `repeat_type<4,Template,Type1,Type2>` with a count of 4 returns `Template<Type1,Type2,Type2,Type2,Type2>`
TEST(RepeatTypeExTest, Repeat4Times) {
  using t = typename repeat_type_ex<4UL, std::tuple, float, std::int64_t>::type;
  ASSERT_EQ((std::tuple_size<t>::value), 5UL);
  ASSERT_TRUE((std::is_same<t, std::tuple<float, std::int64_t, std::int64_t, std::int64_t, std::int64_t>>::value));
}

}  // namespace
