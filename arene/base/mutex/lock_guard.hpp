// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file lock_guard.hpp
/// @brief Exports either a TSA annotated wrapper or a simple alias of std::lock_guard as needed.
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_LOCK_GUARD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_LOCK_GUARD_HPP_

// IWYU pragma: private, include "arene/base/mutex.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

#if ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS) && ARENE_IS_OFF(ARENE_STDLIB_LIBCXX)
#include "arene/base/mutex/detail/lock_guard.hpp"
#else
#include "arene/base/mutex/detail/only_aliases.hpp"
#endif

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

///
/// @brief A lock guard suitable for use with clang's Thread Safety Analysis. Equivalent to @c std::lock_guard.
///
/// If Thread Safety Analysis is not supported by the current compiler, or the current stdlib already has a TSA
/// annotated @c std::lock_guard, then this is a simple alias to @c std::lock_guard. Otherwise it is a thin wrapper
/// around @c std::lock_guard which annotates it as a _scoped capability_, but is otherwise identical to
/// @c std::lock_guard.
///
/// @note The support for Thread Safety Analysis is detected by the @c ARENE_HAS_THREAD_SAFETY_ANALYSIS platform query.
/// @note Currently, only libcxx has an out of the box annotated implementation. Detection is done via the
///       @c ARENE_STDLIB_LIBCXX platform query.
///
/// @tparam Mutex the type of the mutex to guard.
///
template <typename Mutex>
using lock_guard = mutex_detail::selected_lock_guard<Mutex>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_LOCK_GUARD_HPP_
