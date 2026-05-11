// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file decays_to.hpp
/// @brief Provides the @c decays_to_v<T, U> trait
///
///
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_DECAYS_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_DECAYS_TO_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace arene {
namespace base {

/// @brief Determine if one type decays to be the same as another type.
/// @details @c decays_to_v is useful to emulate _deduced-this_ added in C++23
///   and for other disambiguation needs in overload sets
///
/// @code {cpp}
/// decays_to_v<const int&, int> == true;
/// decays_to_v<int&&, int> == true;
/// decays_to_v<int, int> == true;
/// decays_to_v<int*, int> == false;
/// decays_to_v<double, int> == false;
/// @endcode
///
/// @tparam T The type to decay and compare to @c U
/// @tparam U The unqualified type to which the decayed @c T is compared.
///   @c U is not modified before the comparison.
template <class T, class U>
extern constexpr bool decays_to_v = std::is_same<std::decay_t<T>, U>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_DECAYS_TO_HPP_
