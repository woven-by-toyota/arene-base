// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_COPYABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_COPYABLE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/type_traits/is_movable.hpp"

namespace arene {
namespace base {

/// @brief Backport for the C++20 copyable concept
///
/// @tparam T The type to test
///
/// The concept copyable<T> specifies that T is a movable object type that can also be copied (that is, it supports copy
/// construction and copy assignment).
///
/// @{
template <class T, class = arene::base::constraints<>>
extern constexpr bool is_copyable_v = false;

// Note: This is implemented with a constraint on std::is_object as some non-objects are not referenceable (e.g. void).
template <class T>
extern constexpr bool is_copyable_v<T, arene::base::constraints<std::enable_if_t<std::is_object<T>::value>>> =
    std::is_copy_constructible<T>::value && is_movable_v<T> && std::is_assignable<T&, T&>::value &&
    std::is_assignable<T&, T const&>::value && std::is_assignable<T&, T const>::value;
/// @}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_COPYABLE_HPP_
