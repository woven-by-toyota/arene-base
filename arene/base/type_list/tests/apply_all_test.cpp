// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/apply_all.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/tests/type_list_test_common.hpp"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace) aliasing in test helpers to local anon namespace.
using namespace ::arene::base::tests::type_list_tests;

using arene::base::type_lists::apply_all_t;

/// @test `apply_all_t` correctly applies type list duplication and size calculation.
TEST(TypeListTest, ApplyAll) {
  using empty = type_list<>;
  using a = type_list<int, char, bool>;
  using aa = type_list<int, char, bool, int, char, bool>;
  ::testing::StaticAssertTypeEq<apply_all_t<a, duplicate_list>, aa>();

  STATIC_ASSERT_EQ((apply_all_t<empty, size_of>::value), 0U);
  STATIC_ASSERT_EQ((apply_all_t<a, size_of>::value), 3U);
  STATIC_ASSERT_EQ((apply_all_t<aa, size_of>::value), 6U);
}

}  // namespace
