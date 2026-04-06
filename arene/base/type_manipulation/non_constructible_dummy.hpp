// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_NON_CONSTRUCTIBLE_DUMMY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_NON_CONSTRUCTIBLE_DUMMY_HPP_

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
/// @brief A special dummy class that cannot be constructed. It can be used as a substitute argument for copy/move
/// constructors or assignment operators that should be deleted, or any other place where a type is required, but not
/// desired.
class non_constructible_dummy {
 public:
  /// @brief Not default constructible
  explicit non_constructible_dummy() = delete;
  /// @brief Not copyable
  non_constructible_dummy(non_constructible_dummy const&) = delete;
  /// @brief Not movable
  non_constructible_dummy(non_constructible_dummy&&) = delete;
  /// @brief Not copyable
  auto operator=(non_constructible_dummy const&) -> non_constructible_dummy& = delete;
  /// @brief Not movable
  auto operator=(non_constructible_dummy&&) -> non_constructible_dummy& = delete;
  /// @brief Not destructible
  ~non_constructible_dummy() = delete;
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_NON_CONSTRUCTIBLE_DUMMY_HPP_
