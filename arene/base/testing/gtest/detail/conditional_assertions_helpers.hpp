// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_CONDITIONAL_ASSERTIONS_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_CONDITIONAL_ASSERTIONS_HELPERS_HPP_

// IWYU pragma: private, include "arene/base/testing/gtest/conditional_assertions.hpp"
// IWYU pragma: friend "arene/base/testing/gtest/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_traits/always_false.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace testing {

/// @brief An enum to pick a Google Test assertion, used internally to call the correct assertion macro
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
enum class gtest_operation : std::uint8_t {
  assert_true,
  assert_false,
  assert_eq,
  assert_ne,
  assert_lt,
  assert_le,
  assert_gt,
  assert_ge
};
// parasoft-end-suppress AUTOSAR-A2_7_3

namespace testing_detail {

// parasoft-begin-suppress AUTOSAR-M6_2_3-a "False positive: these asserts are not null statements"

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
/// @tparam Operation The type of Google Test assertion to apply to @c Condition
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition, gtest_operation Operation>
class gtest_asserter {
 public:
  static_assert(always_false_v<Condition>, "No Google Test assertion is known for the given operation");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_true> {
 public:
  // parasoft-begin-suppress  AUTOSAR-M5_0_4-a "False positive: no implicit conversion between signed and unsigned"
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "False positive: No string literal is being passed"
  /// @brief Assert that the content of @c Condition is true
  static void do_assert() { ASSERT_TRUE(Condition::body()); }
  // parasoft-end-suppress AUTOSAR-A27_0_4-d "False positive: No string literal is being passed"
  // parasoft-end-suppress  AUTOSAR-M5_0_4-a
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_false> {
 public:
  /// @brief Assert that the content of @c Condition is false
  static void do_assert() { ASSERT_FALSE(Condition::body()); }
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_eq> {
 public:
  // parasoft-begin-suppress  AUTOSAR-M5_0_4-a "False positive: no implicit conversion between signed and unsigned"
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "False positive: No string literal is being passed"
  /// @brief Assert that the left and right halves of @c Condition are equal
  static void do_assert() { ASSERT_EQ(Condition::left(), Condition::right()); }
  // parasoft-end-suppress AUTOSAR-A27_0_4-d "False positive: No string literal is being passed"
  // parasoft-end-suppress  AUTOSAR-M5_0_4-a
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_ne> {
 public:
  /// @brief Assert that the left and right halves of @c Condition are unequal
  static void do_assert() { ASSERT_NE(Condition::left(), Condition::right()); }
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_lt> {
 public:
  /// @brief Assert that the left half of @c Condition is strictly less than the right half
  static void do_assert() { ASSERT_LT(Condition::left(), Condition::right()); }
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_le> {
 public:
  /// @brief Assert that the left half of @c Condition is less than or equal to the right half
  static void do_assert() { ASSERT_LE(Condition::left(), Condition::right()); }
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_gt> {
 public:
  /// @brief Assert that the left half of @c Condition is strictly greater than the right half
  static void do_assert() { ASSERT_GT(Condition::left(), Condition::right()); }
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class that takes a @c Condition created later in this file and calls the correct Google Test assertion
/// @tparam Condition A class holding the condition for an assertion, either unary true/false or a binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class gtest_asserter<Condition, gtest_operation::assert_ge> {
 public:
  /// @brief Assert that the left half of @c Condition is greater than or equal to the right half
  static void do_assert() { ASSERT_GE(Condition::left(), Condition::right()); }
};
// parasoft-end-suppress AUTOSAR-A2_7_3

// parasoft-end-suppress AUTOSAR-M6_2_3-a
// parasoft-begin-suppress CERT_C-PRE31-c "Function calls in *static* asserts do not introduce conditional behavior"

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
/// @tparam Operation The Google Test operation to use in the assertion, either a unary true/false or binary comparison
/// @tparam UseStatic If @c true then @c Condition is statically asserted; otherwise, only when calling @c do_assert
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition, gtest_operation Operation = gtest_operation::assert_true, bool UseStatic = true>
class conditional_asserter : public gtest_asserter<Condition, Operation> {
  static_assert(Condition::body(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_false, true>
    : public gtest_asserter<Condition, gtest_operation::assert_false> {
  static_assert(!Condition::body(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_eq, true>
    : public gtest_asserter<Condition, gtest_operation::assert_eq> {
  static_assert(Condition::left() == Condition::right(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_ne, true>
    : public gtest_asserter<Condition, gtest_operation::assert_ne> {
  static_assert(Condition::left() != Condition::right(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_lt, true>
    : public gtest_asserter<Condition, gtest_operation::assert_lt> {
  static_assert(Condition::left() < Condition::right(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_le, true>
    : public gtest_asserter<Condition, gtest_operation::assert_le> {
  static_assert(Condition::left() <= Condition::right(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_gt, true>
    : public gtest_asserter<Condition, gtest_operation::assert_gt> {
  static_assert(Condition::left() > Condition::right(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition>
class conditional_asserter<Condition, gtest_operation::assert_ge, true>
    : public gtest_asserter<Condition, gtest_operation::assert_ge> {
  static_assert(Condition::left() >= Condition::right(), "Test assertion did not hold at compile time");
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A class which exposes a @c do_assert function from @c gtest_asserter and may also check it at compile time
/// @tparam Condition A class holding the condition of a test assertion, either a unary body or two binary halves
/// @tparam Operation The Google Test operation to use in the assertion, either a unary true/false or binary comparison
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Condition, gtest_operation Operation>
class conditional_asserter<Condition, Operation, false> : public gtest_asserter<Condition, Operation> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

// parasoft-end-suppress CERT_C-PRE31-c

}  // namespace testing_detail

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_CONDITIONAL_ASSERTIONS_HELPERS_HPP_
