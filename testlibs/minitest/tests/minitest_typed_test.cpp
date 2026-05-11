// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

namespace minitest_typed_tests {

template <class T, char>
struct some_user_type {
  operator T() const { return SOME_USER_TYPE_VALUE; }
};

using types_for_testing = ::testing::Types<char, double, some_user_type<int, 'a'>>;

template <typename T>
class TestSuiteForTypedTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForTypedTests, types_for_testing, );

TYPED_TEST(TestSuiteForTypedTests, TypedTest) {
  ASSERT_TRUE(TYPED_TEST_PASSES);
  ASSERT_EQ(0, TypeParam{});
}

using reference_types_for_testing = ::testing::Types<char&, double&, some_user_type<int, 'a'>&>;

template <typename T>
class TestSuiteForRefTypedTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForRefTypedTests, reference_types_for_testing, );

TYPED_TEST(TestSuiteForRefTypedTests, RefTest) { ASSERT_TRUE(REF_TEST_PASSES); }

}  // namespace minitest_typed_tests
