// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_TESTS_CPP14_INLINE_TEST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_TESTS_CPP14_INLINE_TEST_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"

namespace cpp14_inline_test {
namespace cpp14_inline_test_detail {

struct example_add {
  template <typename T>
  constexpr auto operator()(T lhs, T rhs) const -> T {
    return lhs + rhs;
  }
};

}  // namespace cpp14_inline_test_detail

ARENE_CPP14_INLINE_VARIABLE(cpp14_inline_test_detail::example_add, example_add);

}  // namespace cpp14_inline_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_TESTS_CPP14_INLINE_TEST_HPP_
