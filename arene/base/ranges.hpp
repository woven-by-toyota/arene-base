// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file ranges.hpp
/// @brief Public export location of the content from arene/base/ranges
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/ranges/begin_end.hpp"              // IWYU pragma: export
#include "arene/base/ranges/borrowed_iterator.hpp"      // IWYU pragma: export
#include "arene/base/ranges/dangling.hpp"               // IWYU pragma: export
#include "arene/base/ranges/disable_sized_range.hpp"    // IWYU pragma: export
#include "arene/base/ranges/enable_borrowed_range.hpp"  // IWYU pragma: export
#include "arene/base/ranges/is_borrowed_range.hpp"      // IWYU pragma: export
#include "arene/base/ranges/is_range.hpp"               // IWYU pragma: export
#include "arene/base/ranges/size.hpp"                   // IWYU pragma: export
// parasoft-end-suppress AUTOSAR-A16_2_2-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_HPP_
