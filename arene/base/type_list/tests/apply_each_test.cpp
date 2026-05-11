// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/apply_each.hpp"

#include <gtest/gtest.h>

#include "arene/base/type_list/type_list.hpp"

namespace {
using arene::base::type_list;
using arene::base::type_lists::apply_each_t;

template <typename T>
struct wrapped;

/// @test `apply_each_t` correctly applies a template wrapper to the individual elements.
TEST(TypeListTest, ApplyEach) {
  using empty = type_list<>;
  using types = type_list<int, char, bool>;
  using wrapped_once = type_list<wrapped<int>, wrapped<char>, wrapped<bool>>;
  using wrapped_twice = type_list<wrapped<wrapped<int>>, wrapped<wrapped<char>>, wrapped<wrapped<bool>>>;

  ::testing::StaticAssertTypeEq<apply_each_t<empty, wrapped>, empty>();
  ::testing::StaticAssertTypeEq<apply_each_t<types, wrapped>, wrapped_once>();
  ::testing::StaticAssertTypeEq<apply_each_t<wrapped_once, wrapped>, wrapped_twice>();
}

template <typename... Ts>
struct other_type_list;

/// @test `apply_each_t` with distinct nested type list types preserves both without flattening them.
TEST(TypeListTest, ApplyEachNested) {
  using types = type_list<int, char>;
  using nested = apply_each_t<types, other_type_list>;

  ::testing::StaticAssertTypeEq<nested, type_list<other_type_list<int>, other_type_list<char>>>();
}

}  // namespace
