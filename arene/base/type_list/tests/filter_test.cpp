// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/filter.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_class.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::filter_t;

/// @test `filter_t` correctly provides the filtered type list, discriminating integral types from class types.
TEST(TypeListTest, CanRemoveElementsThatDoNotMatchFilter) {
  class x {};
  using rtl1 = type_list<int, std::int16_t, user_defined_type, x, std::uint64_t, double>;

  // "Select integral types"
  ::testing::StaticAssertTypeEq<filter_t<rtl1, std::is_integral>, type_list<int, std::int16_t, std::uint64_t>>();

  // "Select class types"
  ::testing::StaticAssertTypeEq<filter_t<rtl1, std::is_class>, type_list<user_defined_type, x>>();
}

}  // namespace
