// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

namespace {

template <typename T>
class SetUpTearDown : public testing::Test {
 protected:
  bool test_running{false};
 public:
  auto SetUp() -> void override {
    test_running = true;
  }

  auto TearDown() -> void override {
    test_running = false;
  }

  ~SetUpTearDown() {
    if (test_running) {
      testing::assertion_failure(__FILE__, __LINE__, {"Test was set up but not torn down"});
    }
  }
};

using types_for_testing = ::testing::Types<int, double, void>;

// Non-P typed tests work fine and should run SetUp/TearDown.

TYPED_TEST_SUITE(SetUpTearDown, types_for_testing, );

TYPED_TEST(SetUpTearDown, TypedTestDoesSetUpAndTearDown) {
  ASSERT_TRUE(this->test_running);
}

template <typename>
constexpr bool dependent_true{true};

CONDITIONAL_TYPED_TEST(SetUpTearDown, ConditionalTypedTestDoesSetUpAndTearDown, dependent_true<TypeParam>) {
  ASSERT_TRUE(this->test_running);
}

// The various CONSTEXPR test macros can technically be used but they don't instantiate the fixture at compile time.

CONSTEXPR_TYPED_TEST(SetUpTearDown, UnconditionalSuccess) {
  SUCCEED();
}

CONDITIONALLY_CONSTEXPR_TYPED_TEST(SetUpTearDown, ConditionalUnconditionalSuccess, dependent_true<TypeParam>) {
  SUCCEED();
}

// P typed tests work fine and should run SetUp/TearDown.

template <typename T>
class SetUpTearDownP : public SetUpTearDown<T> {};

TYPED_TEST_SUITE_P(SetUpTearDownP);

TYPED_TEST_P(SetUpTearDownP, TypedTestPDoesSetUpAndTearDown) {
  ASSERT_TRUE(this->test_running);
}

REGISTER_TYPED_TEST_SUITE_P(SetUpTearDownP, TypedTestPDoesSetUpAndTearDown);

INSTANTIATE_TYPED_TEST_SUITE_P(MyInstantiation, SetUpTearDownP, types_for_testing, );

}  // namespace
