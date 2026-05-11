// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_HARDWARE_QUERIES_DETAIL_CACHE_LINE_IMPL_32_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_HARDWARE_QUERIES_DETAIL_CACHE_LINE_IMPL_32_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/hardware_queries/.*"

#include "arene/base/stdlib_choice/cstddef.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace compiler_support {
namespace detail {

ARENE_MAYBE_UNUSED constexpr std::size_t l1_cache_line_size{32U};

}  // namespace detail
}  // namespace compiler_support
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_HARDWARE_QUERIES_DETAIL_CACHE_LINE_IMPL_32_HPP_
