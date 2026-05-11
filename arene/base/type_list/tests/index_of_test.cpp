// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/index_of.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::index_of;

/// @test `index_of` correctly returns the index of an element in a type list.
TEST(TypeListTest, TypeListIndexOf) {
  STATIC_ASSERT_EQ((index_of<type_list<int>, int>()), 0);
  STATIC_ASSERT_EQ((index_of<type_list<int, char>, int>()), 0);
  STATIC_ASSERT_EQ((index_of<type_list<char, int>, int>()), 1);
  STATIC_ASSERT_EQ((index_of<type_list<char, int, int const, void>, int const>()), 2);
  STATIC_ASSERT_EQ((index_of<type_list<char, int, void>, void>()), 2);
  STATIC_ASSERT_EQ((index_of<type_list<bool, char, unsigned char, int, unsigned int, double, float>, float>()), 6);

  STATIC_ASSERT_EQ((index_of<type_list<char, void, int, void>, void>()), 1);
  STATIC_ASSERT_EQ((index_of<type_list<void, char, void, int>, void>()), 0);
}

}  // namespace
