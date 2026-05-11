// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/flat_map.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::flatten_t;

/// @test `flatten_t` correctly provides the flattened type list from nested ones.
TEST(TypeListTest, CanFlattenNestedTypeLists) {
  class x {};
  using rtl1 =
      type_list<type_list<int>, type_list<std::int16_t, user_defined_type>, type_list<x, std::uint64_t, double>>;

  // "flatten nested type lists"
  ::testing::
      StaticAssertTypeEq<flatten_t<rtl1>, type_list<int, std::int16_t, user_defined_type, x, std::uint64_t, double>>();

  // "flatten nested type list"
  ::testing::StaticAssertTypeEq<flatten_t<type_list<type_list<int>>>, type_list<int>>();

  // "flatten deeply nested type list"
  ::testing::StaticAssertTypeEq<flatten_t<type_list<rtl1>>, rtl1>();
}

}  // namespace
