// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/priority_tag.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_empty.hpp"
#include "arene/base/testing/gtest.hpp"  // IWYU pragma: keep

namespace {

using ::arene::base::priority_tag;

/// @test Check that priority_tag is default constructible.
CONSTEXPR_TEST(PriorityTagTest, PriorityTagIsDefaultConstructible) {
  CONSTEXPR_ASSERT((std::is_default_constructible<priority_tag<0>>::value));
  CONSTEXPR_ASSERT((std::is_default_constructible<priority_tag<1>>::value));
  CONSTEXPR_ASSERT((std::is_default_constructible<priority_tag<2>>::value));
}

/// @test Check that priority_tag is empty.
CONSTEXPR_TEST(PriorityTagTest, PriorityTagIsEmpty) {
  CONSTEXPR_ASSERT((std::is_empty<priority_tag<0>>::value));
  CONSTEXPR_ASSERT((std::is_empty<priority_tag<1>>::value));
  CONSTEXPR_ASSERT((std::is_empty<priority_tag<2>>::value));
}

/// @test Check that priority_tag higher numbers inherit from lower numbers.
CONSTEXPR_TEST(PriorityTagTest, PriorityTagHigherNumbersInheritFromLower) {
  CONSTEXPR_ASSERT((std::is_base_of<priority_tag<0>, priority_tag<1>>::value));
  CONSTEXPR_ASSERT((std::is_base_of<priority_tag<0>, priority_tag<2>>::value));
  CONSTEXPR_ASSERT((std::is_base_of<priority_tag<0>, priority_tag<3>>::value));

  CONSTEXPR_ASSERT((std::is_base_of<priority_tag<1>, priority_tag<2>>::value));
  CONSTEXPR_ASSERT((std::is_base_of<priority_tag<1>, priority_tag<3>>::value));

  CONSTEXPR_ASSERT((std::is_base_of<priority_tag<2>, priority_tag<3>>::value));
}

}  // namespace
