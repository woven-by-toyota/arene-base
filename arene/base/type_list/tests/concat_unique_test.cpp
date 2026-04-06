// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using arene::base::type_list;
using arene::base::type_lists::concat_t;
using arene::base::type_lists::concat_unique_t;

/// @test `concat_unique_t` correctly provides the concatenated type list, ignoring `empty`.
TEST(TypeListTest, ConcatenateUnique) {
  using empty = type_list<>;
  using ctl1 = type_list<int>;
  using ctl2 = type_list<int, int>;
  using ctl3 = type_list<int, char, bool>;
  using ctl4 = concat_t<empty, empty, ctl1, ctl2, ctl3, ctl1, ctl2, ctl3>;
  // "Concatenating no lists is empty"
  ::testing::StaticAssertTypeEq<concat_unique_t<>, empty>();
  // "Concatenating one empty list is still empty"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty>, empty>();
  // "Concatenating two empty lists is still empty"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty, empty>, empty>();
  // "Concatenating three empty lists is still empty"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty, empty, empty>, empty>();
  // "Concatenating one unique list is that list"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl1>, ctl1>();
  // "Concatenating one non-unique list is that list with duplicates removed"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2>, ctl1>();
  // "Concatenating one unique list is that list"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl3>, ctl3>();
  // "Concatenating one non-unqique list is that list with duplicates removed"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl4>, ctl3>();

  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl1, ctl1>, ctl1>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, ctl2>, ctl1>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, ctl1>, ctl1>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, empty>, ctl1>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty, ctl2>, ctl1>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl3, empty>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty, ctl3>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl4, empty>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty, ctl4>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, ctl3>, ctl3>();
  // "Concatenating two lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl4, ctl2>, type_list<char, bool, int>>();

  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, empty, ctl3>, ctl3>();
  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<empty, ctl2, ctl3>, ctl3>();
  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, ctl3, empty>, ctl3>();

  struct x {};
  struct y {};

  // "Concatenating three lists"
  ::testing::StaticAssertTypeEq<concat_unique_t<ctl2, ctl3, type_list<x, y>>, type_list<int, char, bool, x, y>>();

  // "Concatenating many lists"
  ::testing::StaticAssertTypeEq<
      concat_unique_t<
          ctl2,
          type_list<x, y>,
          ctl1,
          type_list<x, int, y>,
          type_list<bool, char, x>,
          type_list<int, double, y, std::int16_t>,
          type_list<bool, int, ctl1, empty>>,
      type_list<char, x, double, y, std::int16_t, bool, int, ctl1, empty>>();

  // "Concatenating lists of platform dependent types"
  ::testing::StaticAssertTypeEq<INT_AND_INT32_RESULT, concat_unique_t<type_list<int>, type_list<std::int32_t>>>();
}

}  // namespace
