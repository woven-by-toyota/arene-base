// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::inline_string;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

/// @test Invoking `at` on a `const` `inline_string` with an index that is less than or equal to the size of the string
/// is equivalent to invoking `operator[]`
CONSTEXPR_TEST(InlineStringAt, ForInRangeIsEquivalentToIndexOperatorConst) {
  constexpr inline_string<default_size> str{"hello"};
  CONSTEXPR_ASSERT_EQ(str.at(0), str[0]);
  CONSTEXPR_ASSERT_EQ(str.at(2), str[2]);
  CONSTEXPR_ASSERT_EQ(str.at(str.size() - 1), str[str.size() - 1]);
}

/// @test Invoking `at` on a non-`const` `inline_string` with an index that is less than or equal to the size of the
/// string is equivalent to invoking `operator[]`
CONSTEXPR_TEST(InlineStringAt, ForInRangeIsEquivalentToIndexOperatorNonConst) {
  inline_string<default_size> str{"hello"};
  str.at(0) = 'j';
  str.at(4) = 'y';
  CONSTEXPR_ASSERT_EQ(str, "jelly");
}

/// @test Invoking `at` with an index that is larger than the size of the string throws `std::out_of_range`
TEST(InlineStringAt, ForOutOfRangeThrows) {
  inline_string<default_size> str{"hello"};
  ASSERT_THROW(str.at(str.size() + 1) = '0', std::out_of_range);
  auto const& const_str = str;
  ASSERT_THROW(std::ignore = const_str.at(str.size() + 1), std::out_of_range);
}

using MyTypes = ::testing::Types<  //
    inline_string<5>,              //
    inline_string<6>,              //
    inline_string<42>,             //
    inline_string<43>,             //
    inline_string<123>,            //
    inline_string<50000>,          //
    inline_string<100000>,         //
    inline_string<1000000>         //
    >;

template <class T>
struct InlineStringThrow : ::testing::Test {};

TYPED_TEST_SUITE(InlineStringThrow, MyTypes, );

/// @test An @c inline_string throws if the argument of @c at is equal to the @c capacity
TYPED_TEST(InlineStringThrow, AtThrows) {
  auto str1 = TypeParam{"hi"};
  std::ignore = str1.at(0U);
  ASSERT_THROW(str1.at(str1.capacity()), std::out_of_range);

  auto const str2 = TypeParam{"hi"};
  std::ignore = str2.at(0U);
  ASSERT_THROW(str2.at(str2.capacity()), std::out_of_range);

  // NOLINTBEGIN(bugprone-use-after-move)
  auto str3 = TypeParam{"hi"};
  std::ignore = std::move(str3).at(0U);
  ASSERT_THROW(std::move(str3).at(str3.capacity()), std::out_of_range);

  auto const str4 = TypeParam{"hi"};
  std::ignore = std::move(str4).at(0U);
  ASSERT_THROW(std::move(str4).at(str4.capacity()), std::out_of_range);
  // NOLINTEND(bugprone-use-after-move)
}

}  // namespace
