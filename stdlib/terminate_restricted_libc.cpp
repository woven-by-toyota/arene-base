// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/terminate.hpp"

namespace std {

/// @brief function called when exception handling fails
///
/// @note This is an incomplete implementation of @c std::terminate that is only
/// enough to allow use of @c //arene/base:contracts within @c //stdlib.
///
[[noreturn]] void terminate() noexcept {
  while (true) {
      // NOLINTNEXTLINE(hicpp-no-assembler)
      asm volatile("bkpt");
  }
}

}  // namespace std
