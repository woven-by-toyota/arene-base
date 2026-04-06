// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_WITH_GTEST_CONSTEXPR_TEST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_WITH_GTEST_CONSTEXPR_TEST_HPP_

// IWYU pragma: private, include "arene/base/testing/gtest.hpp"
// IWYU pragma: friend "arene/base/testing/gtest/.*"

#include <gtest/gtest.h>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace testing {
namespace internal {

/// @brief dummy function declaration to allow unqualified lookup
///
template <class>
void test_impl() = delete;

/// @brief GoogleTest test type used invoke tests at compile-time
/// @tparam TestTag tag associated with a specific test implementation function
///
template <class TestTag>
class constexpr_test : public ::testing::Test {
 public:
  /// @brief test implementation function
  ///
  /// Invokes the associated test implementation function twice: once at
  /// compile-time and again at runtime.
  ///
  auto TestBody() -> void override {
    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert((test_impl(TestTag{}), true), "test body not invocable in a constant expression");
    // parasoft-end-suppress CERT_C-PRE31-c
    test_impl(TestTag{});
  }
};

}  // namespace internal
}  // namespace testing
}  // namespace base
}  // namespace arene

// AUTOSAR exceptions:
// A16-0-1 The pre-processor shall only be used for unconditional and
// conditional file inclusion and include guards, and using the following
// directives: (1) #ifndef, (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6)
// #else, (7) #define, (8) #endif, (9) #include.

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Function-like macro permitted by A16-0-1 Permit #1"

// parasoft-begin-suppress AUTOSAR-M16_3_1-a "Token pasting '##' is used to
// provide a unique type for use with GoogleTest, a macro based library"

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
/// @brief defines the name of a type associated with a testcase
/// @param testsuite The GoogleTest testsuite name
/// @param testcase The GoogleTest testcase name
///
#define TESTCASE_TYPE_NAME(testsuite, testcase) testsuite##_##testcase

/// @brief defines the name of a tag associated with a testcase
/// @param testsuite The GoogleTest testsuite name
/// @param testcase The GoogleTest testcase name
///
#define TESTCASE_TAG_NAME(testsuite, testcase) testsuite##_##testcase##_tag

// parasoft-end-suppress AUTOSAR-M16_3_1-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Function-like macro permitted by
// A16-0-1 Permit #1"
// This function macro expands '__FILE__' and '__LINE__'.
// This function macro indirectly uses the '##' pre-processor operators to
// manipulate tokens, which is necessary to generate unique identifiers for
// GoogleTest.

// parasoft-begin-suppress AUTOSAR-M16_0_6-a "'testsuite' and 'testcase' cannot
// be parenthesized without breaking macro"

/// @brief defines a test case that is invoked at compile-time
/// @param testsuite The GoogleTest testsuite name
/// @param testcase The GoogleTest testcase name
///
/// Performs custom test registration for testsuite @c testsuite and testcase
/// @testcase that invokes a @c constexpr test implementation function. The
/// implementation function is invoked twice: once at compile-time to ensure it
/// is in-fact compile-time invocable, and again at runtime in order to generate
/// test coverage.
///
#define CONSTEXPR_TEST(testsuite, testcase)                                                                      \
  class TESTCASE_TYPE_NAME(testsuite, testcase);                                                                 \
                                                                                                                 \
  class TESTCASE_TAG_NAME(testsuite, testcase) {                                                                 \
    static ::testing::TestInfo const* const test_info_;                                                          \
  };                                                                                                             \
                                                                                                                 \
  /* NOLINTNEXTLINE(fuchsia-statically-constructed-objects) */                                                   \
  ::testing::TestInfo const* const TESTCASE_TAG_NAME(testsuite, testcase)::test_info_ = ::testing::RegisterTest( \
      ARENE_STRINGIZE(testsuite),                                                                                \
      ARENE_STRINGIZE(testcase),                                                                                 \
      nullptr,                                                                                                   \
      nullptr,                                                                                                   \
      __FILE__,                                                                                                  \
      __LINE__,                                                                                                  \
      [] {                                                                                                       \
        return static_cast<::testing::Test*>(                                                                    \
            new ::arene::base::testing::internal::constexpr_test<TESTCASE_TAG_NAME(testsuite, testcase)>{}       \
        );                                                                                                       \
      }                                                                                                          \
  );                                                                                                             \
                                                                                                                 \
  constexpr void test_impl(TESTCASE_TAG_NAME(testsuite, testcase))

// parasoft-end-suppress AUTOSAR-M16_0_6-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Function-like macro permitted by
// A16-0-1 Permit #1"
// This function macro indirectly uses the '##' pre-processor operators to
// manipulate tokens, which is necessary to generate unique identifiers for
// GoogleTest.

// parasoft-begin-suppress AUTOSAR-M16_0_6-a "'testsuite' and 'testcase' cannot
// be parenthesized without breaking macro"

/// @brief defines a typed test case that is invoked at compile-time
/// @param testsuite The GoogleTest testsuite name
/// @param testcase The GoogleTest testcase name
///
/// Defines a @c TYPED_TEST that invokes a @c constexpr test implementation
/// function. The implementation function is invoked twice: once at compile-time
/// to ensure it is in-fact compile-time invocable, and again at runtime in
/// order to generate test coverage.
///
/// A test defined with this macro may not use @c *this or other non- @c
/// constexpr members from the test fixture.
///
#define CONSTEXPR_TYPED_TEST(testsuite, testcase)                                                                \
  class TESTCASE_TYPE_NAME(testsuite, testcase);                                                                 \
                                                                                                                 \
  class TESTCASE_TAG_NAME(testsuite, testcase) {};                                                               \
                                                                                                                 \
  TYPED_TEST(testsuite, testcase) {                                                                              \
    using ::arene::base::testing::internal::test_impl;                                                           \
    using tag_type = TESTCASE_TAG_NAME(testsuite, testcase);                                                     \
    static_assert((test_impl<TypeParam>(tag_type{}), true), "test body not invocable in a constant expression"); \
    test_impl<TypeParam>(tag_type{});                                                                            \
  }                                                                                                              \
                                                                                                                 \
  template <class TypeParam>                                                                                     \
  constexpr void test_impl(TESTCASE_TAG_NAME(testsuite, testcase))

// parasoft-end-suppress AUTOSAR-M16_0_6-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Function-like macro permitted by
// A16-0-1 Permit #1"
// This function macro indirectly uses the '##' pre-processor operators to
// manipulate tokens, which is necessary to generate unique identifiers for
// GoogleTest.

// parasoft-begin-suppress AUTOSAR-M16_0_6-a "'testsuite' and 'testcase' cannot
// be parenthesized without breaking macro"

/// @brief defines a typed test case that is conditionally invoked at compile-time
/// @param testsuite The GoogleTest testsuite name
/// @param testcase The GoogleTest testcase name
/// @param Condition A condition convertible to @c bool which depends on @c TypeParam
///
/// Defines a @c TYPED_TEST that conditionally invokes a @c constexpr test implementation
/// function. The implementation function is invoked at least once at runtime, and possibly a second time at
/// compile-time, depending on the @c CheckIfShouldRunInConstexpr parameter.
///
/// The @c CheckIfShouldRunInConstexpr parameter can be a type_trait depending on the @c TypeParam of the @c TYPED_TEST.
/// ~~~{.cpp}
/// CONDITIONALLY_CONSTEXPR_TYPED_TEST(
///   TestSuite,
///   TestCase,
///   my_metafunction<TypeParam>{})
/// {
///   ...
/// }
/// ~~~
///
/// A test defined with this macro may not use @c *this or other non- @c
/// constexpr members from the test fixture.
///
#define CONDITIONALLY_CONSTEXPR_TYPED_TEST(testsuite, testcase, ...)                                           \
  class TESTCASE_TYPE_NAME(testsuite, testcase);                                                               \
                                                                                                               \
  class TESTCASE_TAG_NAME(testsuite, testcase) {};                                                             \
                                                                                                               \
  template <class TypeParam>                                                                                   \
  constexpr void                                                                                               \
  maybe_do_test_in_constexpr(TESTCASE_TAG_NAME(testsuite, testcase) tag, std::integral_constant<bool, true>) { \
    using ::arene::base::testing::internal::test_impl;                                                         \
    /* do the test, we don't use @c static_assert due to some gcc8 issues */                                   \
    static_cast<void>(std::integral_constant<bool, (test_impl<TypeParam>(tag), true)>{});                      \
  }                                                                                                            \
                                                                                                               \
  template <class TypeParam>                                                                                   \
  constexpr void                                                                                               \
  maybe_do_test_in_constexpr(TESTCASE_TAG_NAME(testsuite, testcase), std::integral_constant<bool, false>) {}   \
                                                                                                               \
  TYPED_TEST(testsuite, testcase) {                                                                            \
    using ::arene::base::testing::internal::test_impl;                                                         \
    using tag_type = TESTCASE_TAG_NAME(testsuite, testcase);                                                   \
    /* The test_impl() function must be called at runtime before dispatching to maybe_do_test_in_constexpr(),  \
     * otherwise gcc8 will incorrectly think test_impl() is not invocable in a constant expression. */         \
    test_impl<TypeParam>(tag_type{});                                                                          \
    maybe_do_test_in_constexpr<TypeParam>(tag_type{}, std::integral_constant<bool, (__VA_ARGS__)>{});          \
  }                                                                                                            \
                                                                                                               \
  template <class TypeParam>                                                                                   \
  constexpr void test_impl(TESTCASE_TAG_NAME(testsuite, testcase))

/// @brief defines a typed test case that is invoked at compile time which is conditionally run based on the third
/// parameter
/// @param testsuite The GoogleTest testsuite name
/// @param testcase The GoogleTest testcase name
/// @param Condition A condition convertible to @c bool which depends on @c TypeParam
///
/// Defines a @c TYPED_TEST that invokes a @c constexpr test implementation
/// function. The implementation function is invoked twice: once at compile-time
/// to ensure it is in-fact compile-time invocable, and again at runtime in
/// order to generate test coverage. If the @c Condition is false, then the implementation function is never
/// instantiated.
///
/// The @c Condition parameter can be a type_trait depending on the @c TypeParam of the @c TYPED_TEST.
/// ~~~{.cpp}
/// CONDITIONAL_CONSTEXPR_TYPED_TEST(
///   TestSuite,
///   TestCase,
///   my_metafunction<TypeParam>{})
/// {
///   ...
/// }
/// ~~~
///
/// A test defined with this macro may not use @c *this or other non- @c
/// constexpr members from the test fixture.
///
#define CONDITIONAL_CONSTEXPR_TYPED_TEST(testsuite, testcase, ...)                                               \
  class TESTCASE_TYPE_NAME(testsuite, testcase);                                                                 \
                                                                                                                 \
  class TESTCASE_TAG_NAME(testsuite, testcase) {};                                                               \
                                                                                                                 \
  template <class TypeParam>                                                                                     \
  constexpr void maybe_do_test(TESTCASE_TAG_NAME(testsuite, testcase) tag, std::integral_constant<bool, true>) { \
    using ::arene::base::testing::internal::test_impl;                                                           \
                                                                                                                 \
    /* do the test, we don't use @c static_assert due to some gcc8 issues */                                     \
    static_cast<void>(std::integral_constant<bool, (test_impl<TypeParam>(tag), true)>{});                        \
    test_impl<TypeParam>(tag);                                                                                   \
  }                                                                                                              \
                                                                                                                 \
  template <class TypeParam>                                                                                     \
  constexpr void maybe_do_test(TESTCASE_TAG_NAME(testsuite, testcase), std::integral_constant<bool, false>) {    \
    GTEST_SKIP() << "Disabled by condition: " << ARENE_STRINGIZE(__VA_ARGS__);                                   \
  }                                                                                                              \
                                                                                                                 \
  TYPED_TEST(testsuite, testcase) {                                                                              \
    using tag_type = TESTCASE_TAG_NAME(testsuite, testcase);                                                     \
    maybe_do_test<TypeParam>(tag_type{}, std::integral_constant<bool, (__VA_ARGS__)>{});                         \
  }                                                                                                              \
                                                                                                                 \
  template <class TypeParam>                                                                                     \
  constexpr void test_impl(TESTCASE_TAG_NAME(testsuite, testcase))

// parasoft-end-suppress AUTOSAR-M16_0_6-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Defines function-like macros matching the style of GoogleTest assertion
// macros"

/// @brief asserts that the argument is @c true
///
/// Replacement assertion macro for use in @c CONSTEXPR_TEST and @c
/// CONSTEXPR_TYPED_TEST. No GoogleTest assertion macro can be used at
/// compile-time due to all implementations invoking functionality that cannot
/// be used at compile-time (e.g. malloc).
///
#define CONSTEXPR_ASSERT(...) \
  if (not(__VA_ARGS__)) {     \
    FAIL();                   \
  }

/// @brief asserts that the argument is @c false
///
/// Replacement assertion macro for use in @c CONSTEXPR_TEST and @c
/// CONSTEXPR_TYPED_TEST. No GoogleTest assertion macro can be used at
/// compile-time due to all implementations invoking functionality that cannot
/// be used at compile-time (e.g. malloc).
///
#define CONSTEXPR_ASSERT_FALSE(...) CONSTEXPR_ASSERT(!(__VA_ARGS__))

/// @brief asserts that the arguments are equal
///
/// Replacement assertion macro for use in @c CONSTEXPR_TEST and @c
/// CONSTEXPR_TYPED_TEST. No GoogleTest assertion macro can be used at
/// compile-time due to all implementations invoking functionality that cannot
/// be used at compile-time (e.g. malloc).
///
#define CONSTEXPR_ASSERT_EQ(lhs, rhs) CONSTEXPR_ASSERT(((lhs) == (rhs)))

// parasoft-end-suppress AUTOSAR-A16_0_1-d

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_WITH_GTEST_CONSTEXPR_TEST_HPP_
