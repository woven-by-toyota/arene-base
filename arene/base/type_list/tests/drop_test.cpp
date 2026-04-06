// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/drop.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using arene::base::type_list;
using arene::base::type_lists::drop_t;

/// @test `drop_t` removes the first N types from the list
TEST(TypeListDrop, RemoveFirstN) {
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<double, char, int>,
                     drop_t<type_list<double, char, int>, 0>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<char, int>,
                     drop_t<type_list<double, char, int>, 1>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<int>,
                     drop_t<type_list<double, char, int>, 2>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     drop_t<type_list<double, char, int>, 3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     drop_t<type_list<>, 0>>::value);

  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<double, char, int>,
                     drop_t<testing::Types<double, char, int>, 0>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<char, int>,
                     drop_t<testing::Types<double, char, int>, 1>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<int>,
                     drop_t<testing::Types<double, char, int>, 2>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     drop_t<testing::Types<double, char, int>, 3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     drop_t<testing::Types<>, 0>>::value);
}

/// @test `drop_t` handles counts exceeding the size of the list
TEST(TypeListDrop, CountLargerThanList) {
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     drop_t<type_list<double, char, int>, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     type_list<>,
                     drop_t<type_list<>, 1>>::value);

  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     drop_t<testing::Types<double, char, int>, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_same<  //
                     testing::Types<>,
                     drop_t<testing::Types<>, 1>>::value);
}

}  // namespace
