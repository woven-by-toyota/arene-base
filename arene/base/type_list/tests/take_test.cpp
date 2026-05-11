// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/take.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using arene::base::type_list;
using arene::base::type_lists::take_t;

/// @test `take_t` keeps the first N types from the list
TEST(TypeListTake, KeepFirstN) {
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     take_t<type_list<double, char, int>, 0>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<double>,
                     take_t<type_list<double, char, int>, 1>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<double, char>,
                     take_t<type_list<double, char, int>, 2>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<double, char, int>,
                     take_t<type_list<double, char, int>, 3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     take_t<type_list<>, 0>>::value);

  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     take_t<testing::Types<double, char, int>, 0>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<double>,
                     take_t<testing::Types<double, char, int>, 1>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<double, char>,
                     take_t<testing::Types<double, char, int>, 2>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<double, char, int>,
                     take_t<testing::Types<double, char, int>, 3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     take_t<testing::Types<>, 0>>::value);
}

/// @test `take_t` handles counts exceeding the size of the list
TEST(TypeListTake, CountLargerThanList) {
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<double, char, int>,
                     take_t<type_list<double, char, int>, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     take_t<type_list<>, 1>>::value);

  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<double, char, int>,
                     take_t<testing::Types<double, char, int>, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     take_t<testing::Types<>, 1>>::value);
}

}  // namespace
