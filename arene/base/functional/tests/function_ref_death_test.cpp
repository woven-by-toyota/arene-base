// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/functional/function_ref.hpp"

namespace {

using ::arene::base::function_ref;

/// @test The program terminates when a `function_ref` which is not `noexcept` is constructed with a `nullptr`.
TEST(NonNoexceptFunctionRefDeathTest, AttemptingToConstructFromANullFunctionPtrIsAPreconditionViolation) {
  using fun_ptr_type = int (*)(int, int);
  fun_ptr_type ptr = nullptr;
  ASSERT_DEATH(function_ref<int(int, int)>{ptr}, "Precondition");
}

}  // namespace
