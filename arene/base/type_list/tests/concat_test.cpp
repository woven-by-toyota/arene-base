// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using arene::base::type_list;
using arene::base::type_lists::concat_t;

/// @test `concat_t` correctly provides the concatenated type list, ignoring `empty`.
TEST(TypeListTest, ConcatenateLists) {
  using empty = type_list<>;
  using ctl1 = type_list<int>;
  using ctl2 = type_list<int, int>;
  using ctl3 = type_list<int, char, bool>;
  using ctl4 = type_list<empty, empty, ctl1, ctl2, ctl3, ctl1, ctl2, ctl3>;
  // "Concatenating no lists is empty"
  ::testing::StaticAssertTypeEq<concat_t<>, empty>();
  // "Concatenating one empty list is still empty"
  ::testing::StaticAssertTypeEq<concat_t<empty>, empty>();
  // "Concatenating two empty lists is still empty"
  ::testing::StaticAssertTypeEq<concat_t<empty, empty>, empty>();
  // "Concatenating three empty lists is still empty"
  ::testing::StaticAssertTypeEq<concat_t<empty, empty, empty>, empty>();
  // "Concatenating one list is that list"
  ::testing::StaticAssertTypeEq<concat_t<ctl1>, ctl1>();
  // "Concatenating one list is that list"
  ::testing::StaticAssertTypeEq<concat_t<ctl2>, ctl2>();
  // "Concatenating one list is that list"
  ::testing::StaticAssertTypeEq<concat_t<ctl3>, ctl3>();
  // "Concatenating one list is that list"
  ::testing::StaticAssertTypeEq<concat_t<ctl4>, ctl4>();

  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl1, ctl1>, ctl2>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, ctl2>, type_list<int, int, int, int>>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, ctl1>, type_list<int, int, int>>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, empty>, ctl2>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<empty, ctl2>, ctl2>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl3, empty>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<empty, ctl3>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl4, empty>, ctl4>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<empty, ctl4>, ctl4>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, ctl3>, type_list<int, int, int, char, bool>>();
  // "Concatenating two lists"
  ::testing::
      StaticAssertTypeEq<concat_t<ctl4, ctl2>, type_list<empty, empty, ctl1, ctl2, ctl3, ctl1, ctl2, ctl3, int, int>>();

  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, empty, ctl3>, type_list<int, int, int, char, bool>>();
  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_t<empty, ctl2, ctl3>, type_list<int, int, int, char, bool>>();
  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, ctl3, empty>, type_list<int, int, int, char, bool>>();

  struct x {};
  struct y {};

  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_t<ctl2, ctl3, type_list<x, y>>, type_list<int, int, int, char, bool, x, y>>();

  // "Concatenating many lists"
  ::testing::StaticAssertTypeEq<
      concat_t<
          ctl2,
          type_list<x, y>,
          ctl1,
          type_list<x, int, y>,
          type_list<bool, char, x>,
          type_list<int, double, y, std::int16_t>,
          type_list<bool, int, ctl1, empty>>,
      type_list<int, int, x, y, int, x, int, y, bool, char, x, int, double, y, std::int16_t, bool, int, ctl1, empty>>();
}

}  // namespace
