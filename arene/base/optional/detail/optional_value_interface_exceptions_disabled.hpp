// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_EXCEPTIONS_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_EXCEPTIONS_DISABLED_HPP_

// IWYU pragma: private, include "arene/base/optional/detail/optional_value_interface.hpp"

#include "arene/base/detail/exceptions.hpp"

namespace arene {
namespace base {

namespace optional_detail {

/// @brief Helper CRTP class which is a no-op when exceptions are disabled
/// @tparam OptionalT The type of the optional to provide the interface for. Must be an instantiation of @c optional
template <typename OptionalT>
class optional_value_interface {
  static_assert(
      !detail::are_exceptions_enabled_v,
      "can only be instantiated if exceptions are disabled, this suggests a build configuration error."
  );
};

}  // namespace optional_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_EXCEPTIONS_DISABLED_HPP_
