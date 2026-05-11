// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/flat_map.hpp"

#include <gtest/gtest.h>

#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::flat_map_t;

/// @test `flat_map_t` correctly applies a transformation to each element in a type list and flattens the result.
TEST(TypeListTest, FlatMap) {
  using a = type_list<int, char, bool>;
  using aa = type_list<int, int, char, char, bool, bool>;
  ::testing::StaticAssertTypeEq<flat_map_t<a, doubleT>, aa>();
}

}  // namespace
