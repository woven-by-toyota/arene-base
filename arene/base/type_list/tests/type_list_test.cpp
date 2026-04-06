// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_class.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_list/flat_map.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_list/size.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

class incomplete_type;

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::at_t;
using arene::base::type_lists::concat_t;
using arene::base::type_lists::empty;
using arene::base::type_lists::filter_t;
using arene::base::type_lists::flatten_t;
using arene::base::type_lists::remove_duplicates_t;
using arene::base::type_lists::size;

/// @test Undefined (incomplete) types don't generate wrong behavior.
TEST(TypeListTest, IncompleteType) {
  using PAIR = std::pair<std::int32_t, incomplete_type>;

  using T = type_list<std::size_t, PAIR, user_defined_type>;
  // "Size of type_list is wrong"
  STATIC_ASSERT_EQ((size<T>()), 3);
  // "arene::type_list_element<0, T> is not std::size_t"
  ::testing::StaticAssertTypeEq<at_t<T, 0>, std::size_t>();
  // "arene::type_list_element<1, T> is not PAIR"
  ::testing::StaticAssertTypeEq<at_t<T, 1>, PAIR>();
}

/// @test `size` returns the correct size for various type lists.
TEST(TypeListTest, SizeIsCorrect) {
  STATIC_ASSERT_EQ((size<tl1>()), 3UL);
  STATIC_ASSERT_EQ((size<tl2>()), 0UL);
  STATIC_ASSERT_EQ((size<tl3>()), 2UL);
  STATIC_ASSERT_EQ((size<tl4>()), 7UL);
  STATIC_ASSERT_EQ((size<tl5>()), 3UL);
}

/// @test `empty` correctly identifies empty and non-empty type lists.
TEST(TypeListTest, EmptyIsCorrect) {
  STATIC_ASSERT_FALSE((empty<tl1>()));
  STATIC_ASSERT_TRUE((empty<tl2>()));
  STATIC_ASSERT_FALSE((empty<tl3>()));
  STATIC_ASSERT_FALSE((empty<tl4>()));
  STATIC_ASSERT_FALSE((empty<tl5>()));
}

template <class...>
class tl {};

/// @test Verify that various type list operations work with custom class type list.
TEST(TypeListTest, CanUseAnyTypeListType) {
  class x {};
  using rtl1 = tl<int, std::int16_t, user_defined_type, int, x, std::uint64_t, double, double>;

  ::testing::
      StaticAssertTypeEq<remove_duplicates_t<rtl1>, tl<std::int16_t, user_defined_type, int, x, std::uint64_t, double>>(
      );

  ::testing::StaticAssertTypeEq<filter_t<rtl1, std::is_integral>, tl<int, std::int16_t, int, std::uint64_t>>();

  ::testing::StaticAssertTypeEq<filter_t<rtl1, std::is_class>, tl<user_defined_type, x>>();

  ::testing::StaticAssertTypeEq<flatten_t<tl<rtl1, rtl1>>, concat_t<rtl1, rtl1>>();

  ::testing::StaticAssertTypeEq<flatten_t<tl<tl<int>>>, tl<int>>();
}

/// @test Verify that various type list operations work with `std::tuple` type list.
TEST(TypeListTest, CanUseStdTupleTypeListType) {
  class x {};
  using rtl1 = std::tuple<int, std::int16_t, user_defined_type, int, x, std::uint64_t, double, double>;

  ::testing::StaticAssertTypeEq<
      remove_duplicates_t<rtl1>,
      std::tuple<std::int16_t, user_defined_type, int, x, std::uint64_t, double>>();

  ::testing::StaticAssertTypeEq<filter_t<rtl1, std::is_integral>, std::tuple<int, std::int16_t, int, std::uint64_t>>();

  ::testing::StaticAssertTypeEq<filter_t<rtl1, std::is_class>, std::tuple<user_defined_type, x>>();

  ::testing::StaticAssertTypeEq<flatten_t<std::tuple<rtl1, rtl1>>, concat_t<rtl1, rtl1>>();

  ::testing::StaticAssertTypeEq<flatten_t<std::tuple<std::tuple<int>>>, std::tuple<int>>();
}

}  // namespace
