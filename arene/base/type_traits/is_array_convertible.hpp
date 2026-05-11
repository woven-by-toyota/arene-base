// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ARRAY_CONVERTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ARRAY_CONVERTIBLE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"

namespace arene {
namespace base {

/// @brief Type trait to check if a pointer to an array of From is convertible to a
/// pointer to an array of To
template <typename From, typename To>
constexpr bool is_array_convertible_v =
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    std::is_convertible<From (*)[], To (*)[]>::value;

/// @brief Type trait to check if a pointer to an array of From is convertible to a
/// pointer to an array of To
template <typename From, typename To>
class is_array_convertible : public std::integral_constant<bool, is_array_convertible_v<From, To>> {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_ARRAY_CONVERTIBLE_HPP_
