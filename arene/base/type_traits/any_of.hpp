// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file any_of.hpp
/// @brief Provides the @c any_of_v<bool...> trait
///
///
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ANY_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ANY_OF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

///
/// @brief Trait which tests if at least one value in a parameter pack is
/// @c true .
///
/// @tparam Args The set of boolean values to test
///
/// @return true if any value in the parameter pack is @c true
/// @return false if all values in the parameter pack is @c false ,  or the
///         parameter pack is empty.
///
/// @{
template <bool... Args>
extern constexpr bool any_of_v = false;

template <bool B1, bool... Args>
extern constexpr bool any_of_v<B1, Args...> = B1 || any_of_v<Args...>;
/// @}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ANY_OF_HPP_
