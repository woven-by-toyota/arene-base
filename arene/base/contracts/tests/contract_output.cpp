// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <exception>

#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/contracts/contract.hpp"

[[noreturn]] auto foo() -> int {
  ARENE_PRECONDITION(false);
  // even though this never actually executes, the various compilers all disagree on if they can deduce that
  // `ARENE_PRECONDITION(false)` is unconditionally noreturn. Therefore, we unconditionally terminate at this point.
  std::terminate();
}

auto main() -> int { foo(); }
