// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_CONTRACT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_CONTRACT_HPP_

// IWYU pragma: private, include "arene/base/contracts.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// AUTOSAR exceptions:
// A16-0-1 The pre-processor shall only be used for unconditional and
// conditional file inclusion and include guards, and using the following
// directives: (1) #ifndef, (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6)
// #else, (7) #define, (8) #endif, (9) #include.
//
// Exception Rationale: The code below is used to make precondition and
// invariant checks in a way that can be used by everyone, which can thus be
// used to improve safety. The ARENE_DOC_GENERATION_RUNNING macro is also used
// to identify when Doxygen is running and present different definitions to
// Doxygen vs the compiler, in order to give improved documentation.

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Use of function-like macros permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

namespace arene {
namespace base {
namespace contract_detail {

/// @brief The type of contract being checked
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
enum class contract_type : std::uint8_t {
  precondition,  ///< The contract violation is from @c ARENE_PRECONDITION
  invariant,     ///< The contract violation is from @c ARENE_INVARIANT
  unreachable    ///< The contract violation is from @c ARENE_INVARIANT_UNREACHABLE
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A structure holding information about a contract violation
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
struct contract_violation_info {
  /// @brief The filename of the violation location
  detail::raw_c_string file;
  /// @brief The line number of the violation location as a string
  detail::raw_c_string line;
  /// @brief The text form of the message about the violation
  detail::raw_c_string message;
  /// @brief The type of contract
  contract_type type;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Class to create static storage duration values for the contract violation info
/// @tparam Factory A class with a @c arene_contract_make_info function to obtain the violation info
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Factory>
struct info_holder {
  // parasoft-begin-suppress AUTOSAR-M8_5_2-c-2 "False positive: whole value is copied, with brace initialization"
  /// @brief The contract violation info
  static constexpr contract_violation_info value{Factory::arene_contract_make_info()};
  // parasoft-end-suppress AUTOSAR-M8_5_2-c-2
};
// parasoft-end-suppress AUTOSAR-A2_7_3

template <typename Factory>
constexpr contract_violation_info info_holder<Factory>::value;

/// @brief Internal function called when a contract is violated. It outputs the details on standard error and terminates
/// the application.
/// @param info Information about the contract
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
ARENE_NORETURN void contract_violation(contract_violation_info const& info) noexcept;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Wrapper around contract_violation() to allow contracts to be constexpr-compatible on the success path without
/// brining platform-specific implementation details into the header.
/// @param info Information about the violated contract
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
ARENE_NORETURN constexpr void contract_violation_wrapper(contract_violation_info const& info) noexcept {
  // This do-while loop ensures that this function never returns, to satisfy the ARENE_NORETURN annotation
  // It does, however, eventually invoke contract_violation due to the mutually exclusive ifs
  do {  // NOLINT(cppcoreguidelines-avoid-do-while)
    // dummy condition to make invoking non-constexpr contract_violation() a dependent branch, otherwise compilers will
    // consider it a non-constant-expression.
    if (info.file != info.line) {
      contract_violation(info);
    }
    if (info.file == info.line) {
      contract_violation(info);
    }
  } while (true);
}
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace contract_detail
}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "violation_type is the name of an enumeration member; enclosing in
// parentheses would be invalid syntax"
/// Call the contract violation handler for a violated contract.
/// @param violation_type The type of contract violated
/// @param message The violation message
#define ARENE_INTERNAL_CONTRACT_VIOLATION(violation_type, message)                     \
  /*! @brief A class to provide information about the contract violation */            \
  class arene_contract_info_wrapper {                                                  \
   public:                                                                             \
    /*! @brief Return information about the contract violation */                      \
    static constexpr auto arene_contract_make_info() noexcept                          \
        -> ::arene::base::contract_detail::contract_violation_info {                   \
      return {                                                                         \
          static_cast<::arene::base::detail::raw_c_string>(__FILE__),                  \
          static_cast<::arene::base::detail::raw_c_string>(ARENE_STRINGIZE(__LINE__)), \
          static_cast<::arene::base::detail::raw_c_string>(message),                   \
          ::arene::base::contract_detail::contract_type::violation_type                \
      };                                                                               \
    }                                                                                  \
  };                                                                                   \
  ::arene::base::contract_detail::contract_violation_wrapper(                          \
      ::arene::base::contract_detail::info_holder<arene_contract_info_wrapper>::value  \
  )

/// @brief Check if a contract condition is @c true and call the violation handler if not
/// @param violation_type The type of contract being checked
/// @param message The message to report if the condition is not @c true
/// @param condition The condition to check
#define ARENE_INTERNAL_CONTRACT_CHECK(violation_type, message, condition) /*NOLINT*/ \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */                                \
    if (ARENE_EXPECT((condition))) {                                                 \
    } else {                                                                         \
      ARENE_INTERNAL_CONTRACT_VIOLATION(violation_type, (message));                  \
    }                                                                                \
  } while (false)
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2

/// @brief Check that a precondition holds, and terminate the application if it does not.
/// @param condition The macro arguments must be a single expression that can be
/// contextually converted to @c bool. The precondition holds if the expression
/// evaluates to @c true.
#ifdef ARENE_DOC_GENERATION_RUNNING
#define ARENE_PRECONDITION(condition)
#else
#define ARENE_PRECONDITION(...) ARENE_INTERNAL_CONTRACT_CHECK(precondition, #__VA_ARGS__, (__VA_ARGS__))
#endif

/// @brief Check that an invariant holds, and terminate the application if it does not.
/// @param condition The macro arguments must be a single expression that can be
/// contextually converted to @c bool. The invariant holds if the expression
/// evaluates to @c true.
#ifdef ARENE_DOC_GENERATION_RUNNING

#define ARENE_INVARIANT(condition)
#else
#define ARENE_INVARIANT(...) ARENE_INTERNAL_CONTRACT_CHECK(invariant, #__VA_ARGS__, (__VA_ARGS__))
#endif

/// @brief Terminate the application if this line is ever executed. The supplied message will be output in that case.
/// @param message The message to output
#ifdef ARENE_DOC_GENERATION_RUNNING

#define ARENE_INVARIANT_UNREACHABLE(message)
#else
#define ARENE_INVARIANT_UNREACHABLE(message)                   \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */          \
    ARENE_INTERNAL_CONTRACT_VIOLATION(unreachable, (message)); \
  } while (false)

#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_CONTRACT_HPP_
