// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALL_ARE_SAME_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALL_ARE_SAME_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/all_of.hpp"

namespace arene {
namespace base {

/// A bool variable template that is true if all parameters are the same type,
/// otherwise false.
///
/// @note Not an ODR violation to ODR use a variable template definition from
/// multiple translation units.
template <class... T>
extern constexpr bool all_are_same_v = sizeof...(T) < 2;

/// All the types are the same if more than one type and they are all the same
/// as the first.
template <class T, class... Rest>
extern constexpr bool all_are_same_v<T, Rest...> = all_of_v<std::is_same<T, Rest>::value...>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALL_ARE_SAME_HPP_
