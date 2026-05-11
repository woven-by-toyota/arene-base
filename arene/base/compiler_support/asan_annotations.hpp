// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file asan_annotations.hpp
/// @brief Provides a stable interface to asan's public API
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ASAN_ANNOTATIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ASAN_ANNOTATIONS_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

// clang-format off
// formatting is disabled because clang-format doesn't understand the include syntax for HAS_INCLUDE
// parasoft-begin-suppress AUTOSAR-M16_0_7-a "False positive: parasoft doesn't understand the include syntax for HAS_INCLUDE"
#if ARENE_HAS_INCLUDE(<sanitizer/asan_interface.h>) && ARENE_IS_ON(ARENE_ASAN_ENABLED)
#include "arene/base/compiler_support/detail/asan_annotations_enabled.hpp"
#else
#include "arene/base/compiler_support/detail/asan_annotations_disabled.hpp"
#endif
// clang-format on
// parasoft-end-suppress AUTOSAR-M16_0_7-a
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.
// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Conditional defines permitted by A16-0-1 Permit #2"

///
/// @brief Marks a memory region as unaddressable from ASAN's perspective.
/// @param addr The starting address of the region.
/// @param size The size, in bytes, of the memory region to poison.
/// @post If the binary has been compiled under address sanitizer, any attempt to read from the memory location without
///       first marking it as addressable will trigger an ASAN violation. Otherwise it is a noop.
/// @note This is a simple wrapper around ASAN's
///       [public interface](https://github.com/llvm-mirror/compiler-rt/blob/master/include/sanitizer/asan_interface.h).
///       If the platform does not provides @c <sanitizer/asan_interface.h> , this is a noop
/// @warning Due to asan alignment restrictions, this function may only poison a subregion of the input region. In
///          addition, this method is not thread-safe WRT other invocations of ASAN annotations that interact with the
///          same memory region.
///
#define ARENE_ASAN_POISON_MEMORY_REGION(addr, size) ARENE_ASAN_POISON_MEMORY_REGION_IMPL((addr), (size))

///
/// @brief Marks a memory region as addressable from ASAN's perspective.
/// @param addr The starting address of the region.
/// @param size The size, in bytes, of the memory region.
/// @post If the binary has been compiled under address sanitizer, the memory region will have no special considerations
///       from ASAN's perspective. Otherwise it is a noop.
/// @note This is a simple wrapper around ASAN's
///       [public interface](https://github.com/llvm-mirror/compiler-rt/blob/master/include/sanitizer/asan_interface.h).
///       If the platform does not provides @c <sanitizer/asan_interface.h> , this is a noop
/// @warning Due to asan alignment restrictions, this function may only unpoison a subregion of the input region. In
///          addition, this method is not thread-safe WRT other invocations of ASAN annotations that interact with the
///          same memory region.
///
#define ARENE_ASAN_UNPOISON_MEMORY_REGION(addr, size) ARENE_ASAN_UNPOISON_MEMORY_REGION_IMPL((addr), (size))

///
/// @brief Marks a memory region as unaddressable from ASAN's perspective.
/// @param span_of_t An @c arene::base::span<T> defining the range of memory to poison. If @c T is not
///            @c arene::base::byte, then the size of the region to mark will be determined by first converting the span
///            to bytes via @c arene::base::span::as_bytes , and then using the size of that resulting span.
/// @post If the binary has been compiled under address sanitizer, any attempt to read from the memory location without
///       first marking it as addressable will trigger an ASAN violation. Otherwise it is a noop.
/// @note This is a simple wrapper around ASAN's
///       [public interface](https://github.com/llvm-mirror/compiler-rt/blob/master/include/sanitizer/asan_interface.h).
///       If the platform does not provides @c <sanitizer/asan_interface.h> , this is a nop
/// @warning Due to asan alignment restrictions, this function may only poison a subregion of the input region. In
///          addition, this method is not thread-safe WRT other invocations of ASAN annotations that interact with the
///          same memory region.
///
#define ARENE_ASAN_POISON_MEMORY_SPAN(span_of_t) ARENE_ASAN_POISON_MEMORY_SPAN_IMPL(span_of_t)

///
/// @brief Marks a memory region as addressable from ASAN's perspective.
/// @param span_of_t A span defining the range of memory to unpoison. If @c T is not
///            @c arene::base::byte, then the size of the region to mark will be determined by first converting the span
///            to bytes via @c arene::base::span::as_bytes , and then using the size of that resulting span.
/// @post If the binary has been compiled under address sanitizer, the memory region will have no special considerations
///       from ASAN's perspective. Otherwise it is a noop.
/// @note This is a simple wrapper around ASAN's
///       [public interface](https://github.com/llvm-mirror/compiler-rt/blob/master/include/sanitizer/asan_interface.h).
///       If the platform does not provides @c <sanitizer/asan_interface.h> , this is a nop
/// @warning Due to asan alignment restrictions, this function may only unpoison a subregion of the input region. In
///          addition, this method is not thread-safe WRT other invocations of ASAN annotations that interact with the
///          same memory region.
///
#define ARENE_ASAN_UNPOISON_MEMORY_SPAN(span_of_t) ARENE_ASAN_UNPOISON_MEMORY_SPAN_IMPL(span_of_t)

// parasoft-end-suppress AUTOSAR-A16_0_1-d
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ASAN_ANNOTATIONS_HPP_
