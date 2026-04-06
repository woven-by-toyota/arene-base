// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines has_native_handle"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file has_native_handle.hpp
/// @brief Trait for detecting if a mutex has the native_handle API
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_HAS_NATIVE_HANDLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_HAS_NATIVE_HANDLE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/mutex/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/substitution_succeeds.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace mutex_detail {

///
/// @brief SFINAE friendly usage of the @c native_handle() API.
///
/// @tparam Mutex the type to test for a @c native_handle() API's existence.
///
template <typename Mutex>
using use_native_handle = typename Mutex::native_handle_t;

///
/// @brief Trait to detect if a type has a @c native_handle() member function.
///
/// @tparam Mutex the type to test.
/// @return true If @c Mutex::native_handle_t exists.
/// @return false If @c Mutex::native_handle_t does not exist.
///
template <typename Mutex>
constexpr bool has_native_handle = substitution_succeeds<use_native_handle, Mutex>;

///
/// @brief Mixes in a native_handle_t member declaration if the underlying mutex has one.
///
/// @tparam Mutex the mutex to pass through the native handle definition if it exists.
/// @tparam bool
///
template <typename Mutex, bool = has_native_handle<Mutex>>
class mutex_native_handle_member_mixin {
 public:
  /// @brief The native handle type
  using native_handle_t = typename Mutex::native_handle_t;
};

/// @brief Mixes in a native_handle_t member declaration if the underlying mutex has one.
///
/// @tparam Mutex the mutex to pass through the native handle definition if it exists.
template <typename Mutex>
class mutex_native_handle_member_mixin<Mutex, false> {};

}  // namespace mutex_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_HAS_NATIVE_HANDLE_HPP_
