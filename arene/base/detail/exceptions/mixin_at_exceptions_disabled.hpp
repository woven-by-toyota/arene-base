// parasoft-begin-suppress AUTOSAR-A2_8_1-a "This is an implementation of mix_at so contains mixin_at_impl"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_EXCEPTIONS_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_EXCEPTIONS_DISABLED_HPP_

// IWYU pragma: private, include "arene/base/exceptions.hpp"
// IWYU pragma: friend "arene/base/detail/exceptions/.*"

#include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"  // IWYU pragma: keep

namespace arene {
namespace base {
namespace detail {

/// @brief Implementation helper for @c arene::base::detail::mixin_at when exceptions are disabled. It is a no-op.
/// @see arene::base::detail::mixin_at
///
/// @tparam ContainerType the type of the container class to mix into.
/// @tparam IndexType the type of the index used for the @c at() function.
template <typename ContainerType, typename IndexType>
class mixin_at_impl {
  static_assert(
      !are_exceptions_enabled_v,
      "non-throwing mixin_at is only available when exceptions are disabled, this suggests a misconfiguration in build "
      "settings."
  );
};

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_EXCEPTIONS_DISABLED_HPP_
