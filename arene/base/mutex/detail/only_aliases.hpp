// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file only_aliases.hpp
/// @brief Aliases <mutex> content into arene::base when we don't need annotation wrappers.
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/mutex/.*"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_ONLY_ALIASES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_ONLY_ALIASES_HPP_

#include <mutex>

namespace arene {
namespace base {
namespace mutex_detail {

/// @brief A simple alias to @c std::lock_guard<M> if TSA is disabled or is already supported in the stdlib
/// @tparam M The mutex type to lock
template <typename M>
using selected_lock_guard = std::lock_guard<M>;

/// @brief A simple alias to @c std::mutex if TSA is disabled or is already suppored in the stdlib.
using selected_mutex = std::mutex;

/// @brief A simple alias to @c std::timed_mutex if TSA is disabled or is already supported in the stdlib
using selected_timed_mutex = std::timed_mutex;

}  // namespace mutex_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_ONLY_ALIASES_HPP_
