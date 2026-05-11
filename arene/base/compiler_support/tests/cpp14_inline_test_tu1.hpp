// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_TESTS_CPP14_INLINE_TEST_TU1_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_TESTS_CPP14_INLINE_TEST_TU1_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"

namespace cpp14_inline_test {
namespace tu1 {

/// @brief Get the address of the example_add function.
/// @return std::uintptr_t The address of the example_add function.
auto example_add_addr() -> std::uintptr_t;

}  // namespace tu1
}  // namespace cpp14_inline_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_TESTS_CPP14_INLINE_TEST_TU1_HPP_
