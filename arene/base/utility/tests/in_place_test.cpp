// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file in_place_test.cpp
/// @brief unit tests for in_place.hpp
///

#include "arene/base/utility/in_place.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::in_place;
using ::arene::base::in_place_index;
using ::arene::base::in_place_index_t;
using ::arene::base::in_place_t;
using ::arene::base::in_place_type;
using ::arene::base::in_place_type_t;

/// @test @c arene::base::in_place_t is trivial and an empty class.
TEST(InPlace, IsTrivialTagType) {
  STATIC_ASSERT_TRUE(sizeof(in_place_t) == 1);
  STATIC_ASSERT_TRUE(std::is_trivially_copyable<in_place_t>::value);
}
/// @test @c arene::base::in_place is a singleton.
TEST(InPlace, HasSingletonInstance) { ASSERT_EQ(&in_place, &in_place); }

/// @test @c arene::base::in_place can be copied
/// @note This is primarily written to avoid the need to add a single use
/// suppression for C++TestPro.
TEST(InPlace, CanBeCopied) {
  auto const discarded_copy = in_place;
  std::ignore = discarded_copy;
}

template <typename T>
struct InPlaceTypeTest : ::testing::Test {};

struct a_udt {};

using types = ::testing::Types<int, float, in_place_t>;

TYPED_TEST_SUITE(InPlaceTypeTest, types, );
/// @test @c arene::base::in_place_type_t<T> is trivial and an empty class.
TYPED_TEST(InPlaceTypeTest, IsTrivialTagType) {
  STATIC_ASSERT_TRUE(sizeof(in_place_type_t<TypeParam>) == 1);
  STATIC_ASSERT_TRUE(std::is_trivially_copyable<in_place_type_t<TypeParam>>::value);
}
/// @test @c arene::base::in_place_type<T> is a singleton.
TYPED_TEST(InPlaceTypeTest, HasSingletonInstance) { ASSERT_EQ(&in_place_type<TypeParam>, &in_place_type<TypeParam>); }

template <typename T>
struct InPlaceIndexTest : ::testing::Test {};

template <std::size_t I>
struct index : std::integral_constant<std::size_t, I> {};

using indexes = ::testing::Types<index<1>, index<2>, index<3>, index<4>, index<5>, index<6>, index<7>>;

TYPED_TEST_SUITE(InPlaceIndexTest, indexes, );
/// @test @c arene::base::in_place_index_t<Index> is trivial and an empty class.
TYPED_TEST(InPlaceIndexTest, IsTrivialTagType) {
  constexpr auto index = TypeParam{}();
  STATIC_ASSERT_TRUE(sizeof(in_place_index_t<index>) == 1);
  STATIC_ASSERT_TRUE(std::is_trivially_copyable<in_place_index_t<index>>::value);
}
/// @test @c arene::base::in_place_index_t<Index> is a singleton.
TYPED_TEST(InPlaceIndexTest, HasSingletonInstance) {
  constexpr auto index = TypeParam{}();
  ASSERT_EQ(&in_place_index<index>, &in_place_index<index>);
}

}  // namespace
