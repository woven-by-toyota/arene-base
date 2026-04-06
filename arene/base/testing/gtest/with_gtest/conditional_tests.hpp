// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file conditional_tests.hpp
/// @brief Macros to help define test cases that are run or not depending on a generic condition
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_WITH_GTEST_CONDITIONAL_TESTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_WITH_GTEST_CONDITIONAL_TESTS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/testing/gtest/.*"

#include <gtest/gtest.h>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) This code interacts with Google Test's macros so it needs to be too.
// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Defining these macros is necessary because Google Test uses macros"
// parasoft-begin-suppress CERT_C-PRE30-a "Token concatenation is necessary because Google Test does it too"
// parasoft-begin-suppress AUTOSAR-M16_3_1-a "Multi-token concatenation is necessary because Google Test does it too"
// parasoft-begin-suppress AUTOSAR-M16_0_6-a "Some args can not be parenthesized to preserve Google Test's API"

/// @brief Adds a type-parameterized test to a Google Test suite which is conditionally run based on the third parameter
/// @param SuiteName The name of a Google Test suite previously defined with TYPED_TEST_SUITE_P
/// @param TestName The name of this test; needs to be registered later with REGISTER_TYPED_TEST_SUITE_P
/// @param Condition A condition convertible to @c bool which depends on @c TypeParam
#define CONDITIONAL_TYPED_TEST_P(SuiteName, TestName, ...)                                           \
  namespace GTEST_SUITE_NAMESPACE_(SuiteName) {                                                      \
  template <typename TypeParam, bool CondOk>                                                         \
  class ARENE_CONCATENATE(TestName, _Impl)                                                           \
      : public SuiteName<TypeParam> {                                                                \
   private:                                                                                          \
    void TestBody() override {}                                                                      \
  };                                                                                                 \
  template <typename TypeParam_>                                                                     \
  class ARENE_CONCATENATE(TestName, _Impl)<TypeParam_, true> : public SuiteName<TypeParam_> {        \
   private:                                                                                          \
    using TestFixture = SuiteName<TypeParam_>;                                                       \
    using TypeParam = TypeParam_;                                                                    \
    void TestBody() override;                                                                        \
  };                                                                                                 \
  template <typename TypeParam>                                                                      \
  class TestName : public ARENE_CONCATENATE(TestName, _Impl)<TypeParam, (__VA_ARGS__)> {};           \
  ARENE_MAYBE_UNUSED static bool const gtest_##TestName##_defined_ =                                 \
      GTEST_TYPED_TEST_SUITE_P_STATE_(SuiteName)                                                     \
          .AddTestName(__FILE__, __LINE__, GTEST_STRINGIFY_(SuiteName), GTEST_STRINGIFY_(TestName)); \
  } /* namespace GTEST_SUITE_NAMESPACE_(SuiteName) */                                                \
  template <typename TypeParam_Internal_>                                                            \
  void GTEST_SUITE_NAMESPACE_(SuiteName)::ARENE_CONCATENATE(TestName, _Impl)<TypeParam_Internal_, true>::TestBody()

/// @brief Adds a type-parameterized test to a Google Test suite which is conditionally run based on the third parameter
/// @param SuiteName The name of a Google Test suite previously defined with TYPED_TEST_SUITE
/// @param TestName The name of this test
/// @param Condition A condition convertible to @c bool which depends on @c TypeParam
#define CONDITIONAL_TYPED_TEST(testsuite, testcase, ...)                                                        \
  template <typename TypeParam_Internal_>                                                                       \
  class GTEST_TEST_CLASS_NAME_(testsuite, testcase)                                                             \
      : public testsuite<TypeParam_Internal_> {                                                                 \
   private:                                                                                                     \
    using TestFixture = testsuite<TypeParam_Internal_>;                                                         \
    using TypeParam = TypeParam_Internal_;                                                                      \
                                                                                                                \
    void TestBody() override { do_conditional_test(std::integral_constant<bool, (__VA_ARGS__)>{}); }            \
                                                                                                                \
    void do_conditional_test(std::false_type) {                                                                 \
      GTEST_SKIP() << "Disabled by condition: " << ARENE_STRINGIZE(__VA_ARGS__);                                \
    }                                                                                                           \
                                                                                                                \
    void do_conditional_test(std::true_type);                                                                   \
  };                                                                                                            \
  /* NOLINTNEXTLINE(fuchsia-statically-constructed-objects) */                                                  \
  static const bool gtest_##testsuite##_##testcase##_registered_ = ::testing::internal::TypeParameterizedTest<  \
      testsuite,                                                                                                \
      ::testing::internal::TemplateSel<GTEST_TEST_CLASS_NAME_(testsuite, testcase)>,                            \
      GTEST_TYPE_PARAMS_(testsuite)>::                                                                          \
      Register(                                                                                                 \
          "",                                                                                                   \
          ::testing::internal::CodeLocation(__FILE__, __LINE__),                                                \
          GTEST_STRINGIFY_(testsuite),                                                                          \
          GTEST_STRINGIFY_(testcase),                                                                           \
          0,                                                                                                    \
          ::testing::internal::GenerateNames<GTEST_NAME_GENERATOR_(testsuite), GTEST_TYPE_PARAMS_(testsuite)>() \
      );                                                                                                        \
  template <typename TypeParam_Internal_>                                                                       \
  void GTEST_TEST_CLASS_NAME_(testsuite, testcase)<TypeParam_Internal_>::do_conditional_test(std::true_type)

// parasoft-end-suppress AUTOSAR-M16_0_6-a
// parasoft-end-suppress AUTOSAR-M16_3_1-a
// parasoft-end-suppress CERT_C-PRE30-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_WITH_GTEST_CONDITIONAL_TESTS_HPP_
