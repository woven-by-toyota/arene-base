// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file timed_mutex.hpp
/// @brief Exports either a TSA annotated wrapper or a simple alias of std::mutex as needed.
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_TIMED_MUTEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_TIMED_MUTEX_HPP_

// IWYU pragma: private, include "arene/base/mutex.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

#if ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS) && ARENE_IS_OFF(ARENE_STDLIB_LIBCXX)
#include "arene/base/mutex/detail/timed_mutex.hpp"
#else
#include "arene/base/mutex/detail/only_aliases.hpp"
#endif

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

///
/// @brief A timed mutex suitable for use with clang's Thread Safety Analysis. Equivalent to @c std::timed_mutex.
///
/// If Thread Safety Analysis is not supported by the current compiler, or the current stdlib already has a TSA
/// annotated @c std::timed_mutex, then this is a simple alias to @c std::timed_mutex. Otherwise it is a thin wrapper
/// around @c std::timed_mutex which annotates it as a _capability_, but is otherwise identical to @c std::timed_mutex.
///
/// @note The support for Thread Safety Analysis is detected by the @c ARENE_HAS_THREAD_SAFETY_ANALYSIS platform query.
/// @note Currently, only libcxx has an out of the box annotated implementation. Detection is done via the
///       @c ARENE_STDLIB_LIBCXX platform query.
using timed_mutex = mutex_detail::selected_timed_mutex;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_TIMED_MUTEX_HPP_
