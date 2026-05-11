// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file asan_annotations_enabled.hpp
/// @brief Definitions for when a platform provides <sanitizer/asan_interface.h> and we're currently running under asan.
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/asan_annotations.hpp"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_ASAN_ANNOTATIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_ASAN_ANNOTATIONS_ENABLED_HPP_

#include <sanitizer/asan_interface.h>

// NOLINTBEGIN(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.

namespace arene {
namespace base {
namespace compiler_support_detail {

#ifndef ARENE_ASAN_POISON_MEMORY_REGION_IMPL
/// @see ARENE_ASAN_POISON_MEMORY_REGION
#define ARENE_ASAN_POISON_MEMORY_REGION_IMPL(addr, size) ASAN_POISON_MEMORY_REGION(addr, size)
#endif

#ifndef ARENE_ASAN_UNPOISON_MEMORY_REGION_IMPL
/// @see ARENE_ASAN_UNPOISON_MEMORY_REGION
#define ARENE_ASAN_UNPOISON_MEMORY_REGION_IMPL(addr, size) ASAN_UNPOISON_MEMORY_REGION(addr, size)
#endif

#ifndef ARENE_ASAN_POISON_MEMORY_SPAN_IMPL
/// @see ARENE_ASAN_POISON_MEMORY_SPAN
#define ARENE_ASAN_POISON_MEMORY_SPAN_IMPL(span_of_t)                                   \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */                                   \
    const auto asan_poison_memory_span_bytes_span = ::arene::base::as_bytes(span_of_t); \
    ARENE_ASAN_POISON_MEMORY_REGION_IMPL(                                               \
        asan_poison_memory_span_bytes_span.data(),                                      \
        asan_poison_memory_span_bytes_span.size()                                       \
    );                                                                                  \
  } while (false)
#endif

#ifndef ARENE_ASAN_UNPOISON_MEMORY_SPAN_IMPL
/// @see ARENE_ASAN_UNPOISON_MEMORY_SPAN
#define ARENE_ASAN_UNPOISON_MEMORY_SPAN_IMPL(span_of_t)                                   \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */                                     \
    const auto asan_unpoison_memory_span_bytes_span = ::arene::base::as_bytes(span_of_t); \
    ARENE_ASAN_UNPOISON_MEMORY_REGION_IMPL(                                               \
        asan_unpoison_memory_span_bytes_span.data(),                                      \
        asan_unpoison_memory_span_bytes_span.size()                                       \
    );                                                                                    \
  } while (false)
#endif

}  // namespace compiler_support_detail
}  // namespace base
}  // namespace arene

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_ASAN_ANNOTATIONS_ENABLED_HPP_
