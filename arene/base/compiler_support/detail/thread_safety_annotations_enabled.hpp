// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file thread_safety_annotations_enabled.hpp
/// @brief Implementation helper for compiler_support/thread_safety_annotations.hpp for when TSA is supported.
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/thread_safety_annotations.hpp"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_THREAD_SAFETY_ANNOTATIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_THREAD_SAFETY_ANNOTATIONS_ENABLED_HPP_

// NOLINTBEGIN(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.

/// @brief A helper that passes the attribute to @c __attribute__
#define ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(x) __attribute__((x))

// NOLINTEND(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_THREAD_SAFETY_ANNOTATIONS_ENABLED_HPP_
