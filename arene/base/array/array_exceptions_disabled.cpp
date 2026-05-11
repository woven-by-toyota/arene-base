// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array/array.hpp"

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/contracts.hpp"

namespace arene {
namespace base {
namespace array_detail {

/// @brief Calling is an invariant violation as it should not be called when exceptions are disabled.
ARENE_NORETURN void throw_out_of_range() { ARENE_INVARIANT_UNREACHABLE("Array index out of range"); }

}  // namespace array_detail
}  // namespace base
}  // namespace arene
