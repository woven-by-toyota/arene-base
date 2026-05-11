// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_TO_UNDERLYING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_TO_UNDERLYING_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/underlying_type.hpp"

namespace arene {
namespace base {

/// @brief This replicates a function that went into C++23 called @c std::to_underlying (https://wg21.link/P1682). It
/// automatically processes and figures out the type of an enumeration value.
///
/// @remarks Usage sites benefit from this function in that they can never accidentally forget to update the code at
/// several places when it comes to converting between an enumeration and its type. This can avoid problems with casting
/// to the wrong type and getting bad integer promotions or signed -> unsigned (or vice-versa) conversions.
/// @tparam T The type of the numeration for which to return the underlying value
/// @param value The value of type @c T for which to retrieve the underlying value
/// @return The integral value of the supplied enumeration value
template <class T>
constexpr auto to_underlying(T value) noexcept -> std::underlying_type_t<T> {
  return static_cast<std::underlying_type_t<T>>(value);
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_TO_UNDERLYING_HPP_
