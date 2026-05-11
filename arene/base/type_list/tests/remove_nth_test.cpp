// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/remove_nth.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {
/// @test `remove_nth_t` correctly provides the type list without the nth element.
TEST(TypeListTest, CanRemoveNthElement) {
  class x {};
  class y {};
  using rtl1 = arene::base::type_list<int, std::int16_t, int, x, std::uint64_t, double, double, std::int16_t, x, y>;

  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 0>,
      arene::base::type_list<std::int16_t, int, x, std::uint64_t, double, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 1>,
      arene::base::type_list<int, int, x, std::uint64_t, double, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 2>,
      arene::base::type_list<int, std::int16_t, x, std::uint64_t, double, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 3>,
      arene::base::type_list<int, std::int16_t, int, std::uint64_t, double, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 4>,
      arene::base::type_list<int, std::int16_t, int, x, double, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 5>,
      arene::base::type_list<int, std::int16_t, int, x, std::uint64_t, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 6>,
      arene::base::type_list<int, std::int16_t, int, x, std::uint64_t, double, std::int16_t, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 7>,
      arene::base::type_list<int, std::int16_t, int, x, std::uint64_t, double, double, x, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 8>,
      arene::base::type_list<int, std::int16_t, int, x, std::uint64_t, double, double, std::int16_t, y>>();
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<rtl1, 9>,
      arene::base::type_list<int, std::int16_t, int, x, std::uint64_t, double, double, std::int16_t, x>>();
}

/// @test `remove_nth_t` correctly provides the type list without the nth element, even for one element lists.
TEST(TypeListTest, CanRemove0thElementFromOneElementList) {
  ::testing::StaticAssertTypeEq<
      arene::base::type_lists::remove_nth_t<arene::base::type_list<int>, 0>,
      arene::base::type_list<>>();
}

}  // namespace
