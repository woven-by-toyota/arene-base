// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/algorithm"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using min_max_types = arene::base::type_lists::concat_unique_t<
    ::testing::signed_integer_types,
    ::testing::unsigned_integer_types,
    ::testing::floating_point_types>;

template <typename T>
class MinMaxDeathTest : public testing::Test {};

TYPED_TEST_SUITE(MinMaxDeathTest, min_max_types, );

/// @test Trying to call @c std::min on an empty @c std::initializer_list crashes with a precondition violation
TYPED_TEST(MinMaxDeathTest, MinOnEmptyListIsPreconditionViolation) {
  ASSERT_DEATH(std::min(std::initializer_list<TypeParam>{}), "Precondition");
}

/// @test Trying to call @c std::max on an empty @c std::initializer_list crashes with a precondition violation
TYPED_TEST(MinMaxDeathTest, MaxOnEmptyListIsPreconditionViolation) {
  ASSERT_DEATH(std::max(std::initializer_list<TypeParam>{}), "Precondition");
}

}  // namespace
