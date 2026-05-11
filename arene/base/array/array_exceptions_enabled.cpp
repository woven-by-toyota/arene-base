// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array/array.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace array_detail {

/// @brief If exceptions are enabled, throws out of range.
/// @throws std::out_of_range if exceptions are enabled.
ARENE_NORETURN void throw_out_of_range() { throw std::out_of_range("Array index out of range"); }

}  // namespace array_detail
}  // namespace base
}  // namespace arene
