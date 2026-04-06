// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/remove_duplicates.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::remove_duplicates_t;

/// @test `remove_duplicates_t` correctly provides the type list without duplicate elements.
TEST(TypeListTest, CanRemoveDuplicateElements) {
  class x {};
  using rtl1 = type_list<int, std::int16_t, user_defined_type, int, x, std::uint64_t, double, double>;

  // "remove duplicate types"
  ::testing::StaticAssertTypeEq<
      remove_duplicates_t<rtl1>,
      type_list<std::int16_t, user_defined_type, int, x, std::uint64_t, double>>();
}

}  // namespace
