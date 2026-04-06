// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/contracts.hpp"

namespace arene {
namespace base {
namespace inline_map_detail {

/// @brief Implementation helper for conditional disablement of throwing when compiling with `-fno-exceptions`.
ARENE_NORETURN void throw_out_of_range() { ARENE_INVARIANT_UNREACHABLE("Index out of range"); }

}  // namespace inline_map_detail
}  // namespace base
}  // namespace arene
