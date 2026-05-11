// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/reverse.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using ::arene::base::tests::type_list_tests::type_list_types;
namespace type_lists = ::arene::base::type_lists;

template <typename T>
struct TypeListReverseTest : ::testing::Test {};

TYPED_TEST_SUITE(TypeListReverseTest, type_list_types, );

/// @test reversing a multi-element list reverses the order
TYPED_TEST(TypeListReverseTest, ReversesMultipleElements) {
  using tl = typename TypeParam::template type_list_t<double, char, int>;
  using expected = typename TypeParam::template type_list_t<int, char, double>;
  ::testing::StaticAssertTypeEq<expected, type_lists::reverse_t<tl>>();
}

/// @test reversing a single-element list yields the same list
TYPED_TEST(TypeListReverseTest, SingleElementIsIdentity) {
  using tl = typename TypeParam::template type_list_t<int>;
  ::testing::StaticAssertTypeEq<tl, type_lists::reverse_t<tl>>();
}

/// @test reversing an empty list yields an empty list
/// @note @c testing::Types requires at least one element, so this is not a typed test
TEST(TypeListReverse, EmptyListIsIdentity) {
  ::testing::StaticAssertTypeEq<arene::base::type_list<>, type_lists::reverse_t<arene::base::type_list<>>>();
  ::testing::StaticAssertTypeEq<std::tuple<>, type_lists::reverse_t<std::tuple<>>>();
}

/// @test reversing twice yields the original list
TYPED_TEST(TypeListReverseTest, ReversingTwiceIsIdentity) {
  using tl = typename TypeParam::template type_list_t<double, char, int>;
  ::testing::StaticAssertTypeEq<tl, type_lists::reverse_t<type_lists::reverse_t<tl>>>();
}

}  // namespace
