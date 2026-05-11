// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

namespace minitest_typed_tests {

template <class T, char>
struct some_user_type {
  operator T() const { return SOME_USER_TYPE_VALUE; }
};

template <typename T>
class TestSuiteForTypedTests : public testing::Test {};

TYPED_TEST_SUITE_P(TestSuiteForTypedTests);

TYPED_TEST_P(TestSuiteForTypedTests, TypedTest1) {
  ASSERT_TRUE(TYPED_TEST1_PASSES);
  ASSERT_EQ(0, TypeParam{});
}

TYPED_TEST_P(TestSuiteForTypedTests, TypedTest2) {
  ASSERT_TRUE(TYPED_TEST2_PASSES);
  ASSERT_EQ(0, TypeParam{});
  testing::StaticAssertTypeEq<TestFixture, TestSuiteForTypedTests<TypeParam>>();
}

REGISTER_TYPED_TEST_SUITE_P(TestSuiteForTypedTests, TypedTest1, TypedTest2);

template <typename T>
class OtherSuiteForTypedTests : public testing::Test {};

TYPED_TEST_SUITE_P(OtherSuiteForTypedTests);

TYPED_TEST_P(OtherSuiteForTypedTests, TypedTest1) {
  ASSERT_TRUE(sizeof(TypeParam) > MIN_PARAM_SIZE);
  testing::StaticAssertTypeEq<TestFixture, OtherSuiteForTypedTests<TypeParam>>();
}

REGISTER_TYPED_TEST_SUITE_P(OtherSuiteForTypedTests, TypedTest1);

using types_for_testing1 = ::testing::Types<char, double, some_user_type<int, 'a'>>;

INSTANTIATE_TYPED_TEST_SUITE_P(Group1, TestSuiteForTypedTests, types_for_testing1, );
INSTANTIATE_TYPED_TEST_SUITE_P(Group1, OtherSuiteForTypedTests, types_for_testing1, );

using types_for_testing2 = ::testing::Types<int, short, unsigned long long, some_user_type<unsigned, 'X'>>;

INSTANTIATE_TYPED_TEST_SUITE_P(Group2, TestSuiteForTypedTests, types_for_testing2, );

}  // namespace minitest_typed_tests
