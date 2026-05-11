// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file mutex.hpp
/// @brief Public export header for the mutex subpackage
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/mutex/lock_guard.hpp"   // IWYU pragma: export
#include "arene/base/mutex/mutex.hpp"        // IWYU pragma: export
#include "arene/base/mutex/timed_mutex.hpp"  // IWYU pragma: export
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_HPP_
