// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file none_of.hpp
/// @brief Provides the @c non_of_v<bool...> trait
///
///
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_NONE_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_NONE_OF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/type_traits/any_of.hpp"

namespace arene {
namespace base {

///
/// @brief Trait which tests if no values in a parameter pack are @c true .
///
/// @tparam Args The set of boolean values to test
///
/// @return true if no value in the parameter pack is @c true , or the
///         parameter pack is empty.
/// @return false if all values in the parameter pack is @c false .
///
/// @{
template <bool... Args>
constexpr bool none_of_v = !any_of_v<Args...>;
/// @}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_NONE_OF_HPP_
