// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries/arene_debug.hpp"                        // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/asan_enabled.hpp"                       // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"                      // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/coverage_info.hpp"                      // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/exceptions_enabled.hpp"                 // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_builtin.hpp"                        // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_builtin_abs.hpp"                    // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_builtin_atomic.hpp"                 // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_builtin_is_constant_evaluated.hpp"  // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_constexpr_bit_cast.hpp"             // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_feature.hpp"                        // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_include.hpp"                        // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_inline_variables.hpp"               // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_native_char8_t.hpp"                 // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_native_string_view.hpp"             // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_noexcept_function_type.hpp"         // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_starts_ends_with.hpp"               // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_string_contains.hpp"                // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_thread_safety_analysis.hpp"         // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/has_uncaught_exceptions.hpp"            // IWYU pragma: export
#include "arene/base/compiler_support/platform_queries/stdlib_info.hpp"                        // IWYU pragma: export
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HPP_
