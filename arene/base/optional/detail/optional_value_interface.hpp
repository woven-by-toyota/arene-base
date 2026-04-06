// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_HPP_

// IWYU pragma: private, include "arene/base/optional/optional.hpp"
// IWYU pragma: friend "arene/base/optional/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
#include "arene/base/optional/detail/optional_value_interface_exceptions_enabled.hpp"  // IWYU pragma: export
#else
#include "arene/base/optional/detail/optional_value_interface_exceptions_disabled.hpp"  // IWYU pragma: export
#endif
// parasoft-end-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_HPP_
