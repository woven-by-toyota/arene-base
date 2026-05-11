// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file asan_annotations_disabled.hpp
/// @brief Definitions for when a platform does not provide <sanitizer/asan_interface.h>
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/asan_annotations.hpp"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_ASAN_ANNOTATIONS_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_ASAN_ANNOTATIONS_DISABLED_HPP_

// NOLINTBEGIN(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.
// parasoft-begin-suppress AUTOSAR-A16_0_1-a "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Conditional defines permitted by A16-0-1 Permit #2"

#ifndef ARENE_ASAN_POISON_MEMORY_REGION_IMPL
/// @see ARENE_ASAN_POISON_MEMORY_REGION
#define ARENE_ASAN_POISON_MEMORY_REGION_IMPL(addr, size)
#endif

#ifndef ARENE_ASAN_UNPOISON_MEMORY_REGION_IMPL
/// @see ARENE_ASAN_UNPOISON_MEMORY_REGION
#define ARENE_ASAN_UNPOISON_MEMORY_REGION_IMPL(addr, size)
#endif

#ifndef ARENE_ASAN_POISON_MEMORY_SPAN_IMPL
/// @see ARENE_ASAN_POISON_MEMORY_SPAN
#define ARENE_ASAN_POISON_MEMORY_SPAN_IMPL(span_of_t) \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */ \
  } while (false)
#endif

#ifndef ARENE_ASAN_UNPOISON_MEMORY_SPAN_IMPL
/// @see ARENE_ASAN_UNPOISON_MEMORY_SPAN
#define ARENE_ASAN_UNPOISON_MEMORY_SPAN_IMPL(span_of_t) \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */   \
  } while (false)
#endif

// parasoft-end-suppress AUTOSAR-A16_0_1-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_ASAN_ANNOTATIONS_DISABLED_HPP_
