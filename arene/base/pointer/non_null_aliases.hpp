// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_NULL_ALIASES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_NULL_ALIASES_HPP_

// IWYU pragma: private, include "arene/base/pointer.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include "arene/base/pointer/detail/non_null_aliases.hpp"  // IWYU pragma: export
#endif
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_NULL_ALIASES_HPP_
