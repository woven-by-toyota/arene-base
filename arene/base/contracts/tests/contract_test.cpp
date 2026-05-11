// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/contract.hpp"

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/contracts/detail/stacktrace/stacktrace.hpp"
#include "arene/base/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::contract_detail::contract_type;
using ::arene::base::contract_detail::contract_violation_info;
using ::arene::base::contract_detail::contract_violation_wrapper;

/// @test Invoking `contract_violation_wrapper` terminates the program, whichever violation type is passed
TEST(ContractViolationWrapperDeathTest, CallsAbortForAllContractTypes) {
  contract_violation_info info{"test_file", "test_line", "test_message", contract_type::invariant};
  for (auto const type : {contract_type::invariant, contract_type::precondition, contract_type::unreachable}) {
    info.type = type;
    EXPECT_DEATH(contract_violation_wrapper(info), "");
  }
}

/// @test When `contract_violation_wrapper` is invoked, the the filename specified in the contract info is printed to
/// @c std::err before the program is terminated.
TEST(ContractViolationWrapperDeathTest, PrintsFileFromInfoForAllContractTypes) {
  auto const* file = "test_file";
  contract_violation_info info{file, "test_line", "test_message", contract_type::invariant};
  for (auto const type : {contract_type::invariant, contract_type::precondition, contract_type::unreachable}) {
    info.type = type;
    EXPECT_DEATH(contract_violation_wrapper(info), file);
  }
}

/// @test When `contract_violation_wrapper` is invoked, the the line number specified in the contract info is printed to
/// @c std::err before the program is terminated.
TEST(ContractViolationWrapperDeathTest, PrintsLineFromInfoForAllContractTypes) {
  auto const* line = "test_line";
  contract_violation_info info{"test_file", line, "test_message", contract_type::invariant};
  for (auto const type : {contract_type::invariant, contract_type::precondition, contract_type::unreachable}) {
    info.type = type;
    EXPECT_DEATH(contract_violation_wrapper(info), line);
  }
}

/// @test When `contract_violation_wrapper` is invoked, the the message specified in the contract info is printed to
/// @c std::err before the program is terminated.
TEST(ContractViolationWrapperDeathTest, PrintsMessageFromInfoForAllContractTypes) {
  auto const* message = "test_message";
  contract_violation_info info{"test_file", "test_line", message, contract_type::invariant};
  for (auto const type : {contract_type::invariant, contract_type::precondition, contract_type::unreachable}) {
    info.type = type;
    EXPECT_DEATH(contract_violation_wrapper(info), message);
  }
}

/// @test When `contract_violation_wrapper` is invoked, a stringified representation of the violation type specified in
/// the contract info is printed to @c std::err before the program is terminated.
TEST(ContractViolationWrapperDeathTest, PrintsTypeOfViolationForAllContractTypes) {
  contract_violation_info info{"test_file", "test_line", "test_message", contract_type::invariant};
  {
    SCOPED_TRACE("Precondition");
    info.type = contract_type::precondition;
    EXPECT_DEATH(contract_violation_wrapper(info), "Precondition");
  }
  {
    SCOPED_TRACE("Invariant");
    info.type = contract_type::invariant;
    EXPECT_DEATH(contract_violation_wrapper(info), "Invariant");
  }
  {
    SCOPED_TRACE("Unreachable");
    info.type = contract_type::unreachable;
    EXPECT_DEATH(contract_violation_wrapper(info), "Unreachable");
  }
}

/// @test When `contract_violation_wrapper` is invoked, a stringified representation of the violation type specified in
/// the contract info is printed to @c std::err before the program is terminated.
TEST(ContractViolationWrapperDeathTest, PrintsStacktraceForAllContractTypes) {
  contract_violation_info info{"test_file", "test_line", "test_message", contract_type::invariant};

  static_assert(
      arene::base::string_view{__FILE__}.ends_with("contract_test.cpp"),
      "Test assumes that this file is called \"contract_test.cpp\"."
  );
  constexpr auto filename = "contract_test";

  if (arene::base::contracts_detail::stacktrace_is_noop) {
    // The else branch tests for the presence of @c filename in the stacktrace. Therefore, @c filename should not be
    // part of the noop stacktrace. Otherwise, testing for @c filename is moot.
    {
      SCOPED_TRACE("Precondition");
      info.type = contract_type::precondition;
      EXPECT_DEATH(contract_violation_wrapper(info), ::testing::Not(::testing::HasSubstr(filename)));
    }
    {
      SCOPED_TRACE("Invariant");
      info.type = contract_type::invariant;
      EXPECT_DEATH(contract_violation_wrapper(info), ::testing::Not(::testing::HasSubstr(filename)));
    }
    {
      SCOPED_TRACE("Unreachable");
      info.type = contract_type::unreachable;
      EXPECT_DEATH(contract_violation_wrapper(info), ::testing::Not(::testing::HasSubstr(filename)));
    }
  } else {
    // Stacktraces are enabled: assume that @c filename is part of the stacktrace
    {
      SCOPED_TRACE("Precondition");
      info.type = contract_type::precondition;
      EXPECT_DEATH(contract_violation_wrapper(info), filename);
    }
    {
      SCOPED_TRACE("Invariant");
      info.type = contract_type::invariant;
      EXPECT_DEATH(contract_violation_wrapper(info), filename);
    }
    {
      SCOPED_TRACE("Unreachable");
      info.type = contract_type::unreachable;
      EXPECT_DEATH(contract_violation_wrapper(info), filename);
    }
  }
}

/// @test Invoking `contract_violation_wrapper` terminates the program, even if the filename string and line number
/// string are the same in the provided contract info.
TEST(ContractViolationWrapperDeathTest, IfFileEqualsLineStillAborts) {
  // This test is needed to get branch coverage due to the dependent branch trick used in the implementation.
  auto const* bad_payload = "test_message";
  contract_violation_info info{bad_payload, bad_payload, bad_payload, contract_type::invariant};
  for (auto const type : {contract_type::invariant, contract_type::precondition, contract_type::unreachable}) {
    info.type = type;
    EXPECT_DEATH(contract_violation_wrapper(info), "");
  }
}

/// @test Invoking `contract_violation_wrapper` terminates the program, and the file, line and message are present in
/// the output, even if the violation type is not a valid enumeration value
TEST(ContractViolationWrapperDeathTest, InvalidContractTypeStillPrintsContentOtherThanContractType) {
  auto const* file = "test_file";
  auto const* line = "test_line";
  auto const* message = "test_message";
  contract_violation_info const info{
      file,
      line,
      message,
      static_cast<contract_type>(std::numeric_limits<std::uint8_t>::max())
  };
  EXPECT_DEATH(
      contract_violation_wrapper(info),
      ::testing::AllOf(::testing::HasSubstr(file), ::testing::HasSubstr(line), ::testing::HasSubstr(message))
  );
}

constexpr auto pre_must_be_non_zero_direct(int const value) -> bool {
  ARENE_PRECONDITION(value != 0);
  return true;
}

constexpr auto is_non_zero(int const value) -> bool { return value != 0; }

constexpr auto pre_must_be_non_zero_indirect(int const value) -> bool {
  ARENE_PRECONDITION(is_non_zero(value));
  return true;
}

/// @test Invoking a function with a precondition which is not violated does not terminate the program.
TEST(PreconditionNonViolated, IsNop) {
  for (auto const value : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, std::numeric_limits<int>::max()}) {
    EXPECT_NO_FATAL_FAILURE(pre_must_be_non_zero_direct(value););
    EXPECT_NO_FATAL_FAILURE(pre_must_be_non_zero_indirect(value););
  }
}

/// @test A function marked `constexpr` can use `ARENE_PRECONDITION`, and be invoked in a constant expression if the
/// precondition is not violated
TEST(PreconditionNonViolated, CanBeInvokedConstexpr) {
  STATIC_ASSERT_TRUE(pre_must_be_non_zero_direct(1));
  STATIC_ASSERT_TRUE(pre_must_be_non_zero_indirect(1));
}

/// @test Invoking a function with a precondition which is violated terminates the program.
TEST(PreconditionViolatedDeathTest, Aborts) {
  EXPECT_DEATH(pre_must_be_non_zero_direct(0), "");
  EXPECT_DEATH(pre_must_be_non_zero_indirect(0), "");
}

/// @test When an @c ARENE_PRECONDITION is violated, the the filename and line number containing the precondition
/// is printed to @c std::err before the program is terminated.
TEST(PreconditionViolatedDeathTest, PrintsFileAndLine) {
  std::string const format;
  /////////////////////////////////////////
  // NOTE: these tests are rather brittle, because we have to hard-code the line the precondition is on. If the content
  // of this file changes and these tests start failing, you will probably have to update these lines.
  EXPECT_DEATH(pre_must_be_non_zero_direct(0), ::testing::HasSubstr((format + __FILE__ + ":" + ARENE_STRINGIZE(169))));
  EXPECT_DEATH(
      pre_must_be_non_zero_indirect(0),
      ::testing::HasSubstr((format + __FILE__ + ":" + ARENE_STRINGIZE(176)))
  );
  /////////////////////////////////////////
}

/// @test When an @c ARENE_PRECONDITION is violated, a stringified representation of the expression passed to the the
/// precondition is printed to @c std::err before the program is terminated.
TEST(PreconditionViolatedDeathTest, PrintsPredicateStringized) {
  EXPECT_DEATH(pre_must_be_non_zero_direct(0), ::testing::HasSubstr("value != 0"));
  EXPECT_DEATH(pre_must_be_non_zero_indirect(0), ::testing::HasSubstr("is_non_zero(value)"));
}

/// @test When an @c ARENE_PRECONDITION is violated, the text `"Precondition"` is printed to @c std::err before the
/// program is terminated.
TEST(PreconditionViolatedDeathTest, PrintsPreconditionAsCause) {
  EXPECT_DEATH(pre_must_be_non_zero_direct(0), ::testing::HasSubstr("Precondition"));
  EXPECT_DEATH(pre_must_be_non_zero_indirect(0), ::testing::HasSubstr("Precondition"));
}

constexpr auto inv_must_be_non_zero_direct(int const value) -> bool {
  ARENE_INVARIANT(value != 0);
  return true;
}

constexpr auto inv_must_be_non_zero_indirect(int const value) -> bool {
  ARENE_INVARIANT(is_non_zero(value));
  return true;
}

/// @test Invoking a function with an invariant which is not violated does not terminate the program.
TEST(InvariantNonViolated, IsNop) {
  for (auto const value : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, std::numeric_limits<int>::max()}) {
    EXPECT_NO_FATAL_FAILURE(inv_must_be_non_zero_direct(value););
    EXPECT_NO_FATAL_FAILURE(inv_must_be_non_zero_indirect(value););
  }
}

/// @test A function marked `constexpr` can use `ARENE_INVARIANT`, and be invoked in a constant expression if the
/// invariant is not violated
TEST(InvariantNonViolated, CanBeInvokedConstexpr) {
  STATIC_ASSERT_TRUE(inv_must_be_non_zero_direct(1));
  STATIC_ASSERT_TRUE(inv_must_be_non_zero_indirect(1));
}

/// @test Invoking a function with an invariant which is violated terminates the program.
TEST(InvariantViolatedDeathTest, Aborts) {
  EXPECT_DEATH(inv_must_be_non_zero_direct(0), "");
  EXPECT_DEATH(inv_must_be_non_zero_indirect(0), "");
}

/// @test When an @c ARENE_INVARIANT is violated, the the filename and line number containing the invariant
/// is printed to @c std::err before the program is terminated.
TEST(InvariantViolatedDeathTest, PrintsFileAndLine) {
  std::string const format;
  /////////////////////////////////////////
  // NOTE: these tests are rather brittle, because we have to hard-code the line the precondition is on. If the content
  // of this file changes and these tests start failing, you will probably have to update these lines.
  EXPECT_DEATH(inv_must_be_non_zero_direct(0), ::testing::HasSubstr((format + __FILE__ + ":" + ARENE_STRINGIZE(231))));
  EXPECT_DEATH(
      inv_must_be_non_zero_indirect(0),
      ::testing::HasSubstr((format + __FILE__ + ":" + ARENE_STRINGIZE(236)))
  );
  /////////////////////////////////////////
}

/// @test When an @c ARENE_INVARIANT is violated, a stringified representation of the expression passed to the the
/// invariant is printed to @c std::err before the program is terminated.
TEST(InvariantViolatedDeathTest, PrintsPredicateStringized) {
  EXPECT_DEATH(inv_must_be_non_zero_direct(0), ::testing::HasSubstr("value != 0"));
  EXPECT_DEATH(inv_must_be_non_zero_indirect(0), ::testing::HasSubstr("is_non_zero(value)"));
}

/// @test When an @c ARENE_INVARIANT is violated, the text `"Invariant"` is printed to @c std::err before the
/// program is terminated.
TEST(InvariantViolatedDeathTest, PrintsInvariantAsCause) {
  EXPECT_DEATH(inv_must_be_non_zero_direct(0), ::testing::HasSubstr("Invariant"));
  EXPECT_DEATH(inv_must_be_non_zero_indirect(0), ::testing::HasSubstr("Invariant"));
}

constexpr auto* unreachable_message = "unreachable message";

constexpr auto unreachable_must_be_non_zero(int const value) -> bool {
  if (value != 0) {
    return true;
  }
  ARENE_INVARIANT_UNREACHABLE(unreachable_message);
}

/// @test Invoking a function with an unreachable line which is not reached does not terminate the program.
TEST(UnreachableNonViolated, IsNop) {
  for (auto const value : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, std::numeric_limits<int>::max()}) {
    EXPECT_NO_FATAL_FAILURE(unreachable_must_be_non_zero(value););
  }
}

/// @test A function marked `constexpr` can use `ARENE_INVARIANT_UNREACHABLE`, and be invoked in a constant expression
/// if the unreachable line is not reached
TEST(UnreachableNonViolated, CanBeInvokedConstexpr) { STATIC_ASSERT_TRUE(unreachable_must_be_non_zero(1)); }

/// @test Invoking a function with a line marked as unreachable which is reached terminates the program.
TEST(UnreachableViolatedDeathTest, Aborts) { EXPECT_DEATH(unreachable_must_be_non_zero(0), ""); }

/// @test When an @c ARENE_INVARIANT_UNREACHABLE is violated, the the filename and line containing the unreachable
/// is printed to @c std::err before the program is terminated.
TEST(UnreachableViolatedDeathTest, PrintsFileAndLine) {
  std::string const format;
  /////////////////////////////////////////
  // NOTE: these tests are rather brittle, because we have to hard-code the line the precondition is on. If the content
  // of this file changes and these tests start failing, you will probably have to update these lines.
  EXPECT_DEATH(unreachable_must_be_non_zero(0), ::testing::HasSubstr((format + __FILE__ + ":" + ARENE_STRINGIZE(296))));
  /////////////////////////////////////////
}

/// @test When an @c ARENE_INVARIANT_UNREACHABLE is violated, a stringified representation of the expression passed to
/// the macro is printed to @c std::err before the program is terminated.
TEST(UnreachableViolatedDeathTest, PrintsMessage) {
  EXPECT_DEATH(unreachable_must_be_non_zero(0), ::testing::HasSubstr(unreachable_message));
}

/// @test When an @c ARENE_INVARIANT_UNREACHABLE is violated, the text `"Unreachable"` is printed to @c std::err before
/// the program is terminated.
TEST(UnreachableViolatedDeathTest, PrintsInvariantAsCause) {
  EXPECT_DEATH(unreachable_must_be_non_zero(0), ::testing::HasSubstr("Unreachable"));
}

}  // namespace
