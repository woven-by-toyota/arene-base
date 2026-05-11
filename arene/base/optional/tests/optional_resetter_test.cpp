// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/optional/optional_resetter.hpp"

#include <gtest/gtest.h>

#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::optional;
using ::arene::base::optional_resetter;

template <typename T>
struct OptionalResetterTest : public ::testing::Test {};

using types = ::testing::Types<int, double, arene::base::string_view>;

TYPED_TEST_SUITE(OptionalResetterTest, types, );

/// @test `optional_resetter` is not default constructible.
TYPED_TEST(OptionalResetterTest, IsNotDefaultConstructible) {
  STATIC_ASSERT_FALSE(std::is_default_constructible<optional_resetter<TypeParam>>::value);
}

/// @test `optional_resetter` is not copy-constructible or copy-assignable.
TYPED_TEST(OptionalResetterTest, IsNotCopyable) {
  STATIC_ASSERT_FALSE(std::is_copy_constructible<optional_resetter<TypeParam>>::value);
  STATIC_ASSERT_FALSE(std::is_copy_assignable<optional_resetter<TypeParam>>::value);
}

/// @test `optional_resetter` is not move-constructible or move-assignable.
TYPED_TEST(OptionalResetterTest, IsNotMovable) {
  STATIC_ASSERT_FALSE(std::is_move_constructible<optional_resetter<TypeParam>>::value);
  STATIC_ASSERT_FALSE(std::is_move_assignable<optional_resetter<TypeParam>>::value);
}

/// @test `optional_resetter` resets the `optional` it holds on destruction if the `optional_resetter` was not
/// dismissed. The reset `optional` holds no value.
TYPED_TEST(OptionalResetterTest, ResetsHeldOptionalOnDestructionIfNotReset) {
  optional<TypeParam> will_be_reset{TypeParam{}};
  {  // start scope
    optional_resetter<TypeParam> const resetter{will_be_reset};
  }  // end scope
  EXPECT_FALSE(will_be_reset.has_value());
}

/// @test `optional_resetter` does not reset the `optional` it holds on destruction if the `optional_resetter` was
/// dismissed. The held `optional` still holds a value if it originally held one.
TYPED_TEST(OptionalResetterTest, DoesNotResetHeldOptionalOnDestructionIfReset) {
  optional<TypeParam> will_not_be_reset{TypeParam{}};
  {  // start scope
    optional_resetter<TypeParam> resetter{will_not_be_reset};
    resetter.dismiss();
  }  // end scope
  EXPECT_TRUE(will_not_be_reset.has_value());
}

}  // namespace
