// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/at.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::at_t;

/// @test `at_t` correctly provides the index-th element from a type list.
TEST(TypeListTest, TypeListElement) {
  // "Can get 1st element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl1, 0>, std::int32_t>();
  // "Can get 2nd element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl1, 1>, std::int8_t>();
  // "Can get 3rd element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl1, 2>, user_defined_type>();
  // "Can get 1st element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl3, 0>, user_defined_type>();
  // "Can get 2nd element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl3, 1>, user_defined_type>();
  // "Can get 1st element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl5, 0>, tl1>();
  // "Can get 2nd element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl5, 1>, tl2>();
  // "Can get 3rd element of type list"
  ::testing::StaticAssertTypeEq<at_t<tl5, 2>, tl3>();
}

}  // namespace
