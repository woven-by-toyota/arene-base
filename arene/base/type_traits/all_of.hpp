// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file all_of.hpp
/// @brief Provides the @c all_of_v<bool...> trait
///
///
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALL_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALL_OF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"
namespace arene {
namespace base {

///
/// @brief Trait which tests if every value in a parameter pack is @c true .
///
/// @tparam Args The set of boolean values to test
///
/// @return true if every value in the parameter pack is @c true , or the
///         parameter pack is empty.
/// @return false if any value in the parameter pack is @c false .
///
/// @{
template <bool... Args>
extern constexpr bool all_of_v = true;

template <bool B1, bool... Args>
extern constexpr bool all_of_v<B1, Args...> = B1 && all_of_v<Args...>;
/// @}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALL_OF_HPP_
