// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_HARDWARE_QUERIES_CACHE_LINE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_HARDWARE_QUERIES_CACHE_LINE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/hardware_queries/.*"

#include "arene/base/stdlib_choice/cstddef.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// To respect AUTOSAR C++14 constraints on C++ preprocessor usage A16-0-1
// We do use preprocessor only to conditionally include a file

#if (defined(__arm__) && defined(__ARM_ARCH_5T__))

#include "arene/base/compiler_support/hardware_queries/detail/cache_line_impl_32.hpp"  // IWYU pragma: export

#else
// Default value to 64 bytes
// Should be a good fit most of the cases (x86_64, x86, aarch64)

#include "arene/base/compiler_support/hardware_queries/detail/cache_line_impl_64.hpp"  // IWYU pragma: export

#endif

namespace arene {
namespace base {

/// @brief The minimum memory spacing required between two items to avoid false-sharing
///
/// Backport of C++17's
/// [std::hardware_destructive_interference_size](https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size).
/// See [P0154R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0154r0.html) for more details
ARENE_MAYBE_UNUSED constexpr std::size_t hardware_destructive_interference_size{
    compiler_support::detail::l1_cache_line_size
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_HARDWARE_QUERIES_CACHE_LINE_HPP_
