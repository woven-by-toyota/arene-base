// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/remove_duplicates.hpp"

#include <gtest/gtest.h>

#include "arene/base/functional/bind_back.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/not_equal_to.hpp"
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

/// @test @c remove_duplicates uses @c bind_back with @c std::not_equal_to. We
/// exercise it here at run-time to avoid missing line coverage due to
/// unexecuted instantiations.
TEST(TypeListTest, BindBackWithNotEqualTo) {
  // this intentionally uses a non-const lvalue reference for coverage
  auto not_equal_to_zero = arene::base::bind_back(std::not_equal_to<>{}, std::size_t{});

  ASSERT_TRUE(not_equal_to_zero(std::size_t{1}));
  ASSERT_FALSE(not_equal_to_zero(std::size_t{}));

  auto const one = std::size_t{1};
  ASSERT_TRUE(not_equal_to_zero(one));
}

}  // namespace
