// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::is_transparent_comparator_for"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file is_transparent_comparator_for.hpp
/// @brief Provides implementation of arene::base::is_transparent_comparator_for
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_TRANSPARENT_COMPARATOR_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_TRANSPARENT_COMPARATOR_FOR_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {

/// @brief Trait to determine if a given type is a _transparent comparator_.
///
/// @tparam C The type to test
///
/// Value will be @c true if @c C::is_transparent exists and is a type definition, else @c false
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename C, typename = void>
class is_transparent_comparator : public std::false_type {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Trait to determine if a given type is a _transparent comparator_.
///
/// This specialization handles the case that @c C is a transparent comparator
///
/// @tparam C The type to test
///
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename C>
class is_transparent_comparator<C, void_t<typename C::is_transparent>> : public std::true_type {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Trait to determine if a given type is a _transparent comparator_.
///
/// @tparam C The type to test
/// @return true If @c C::is_transparent exists and is a type definition
/// @return false If @c C::is_transparent does not exist, or is not a type definition.
template <typename C>
extern constexpr bool is_transparent_comparator_v = is_transparent_comparator<C>::value;

/// @brief Trait to determine if a given type is a _transparent comparator_ for a given set of operands.
///
/// @tparam C The type to test
/// @tparam T The type of the left hand operand
/// @tparam U The type of the right hand operand
/// @return bool Will be equivalent to <c>is_transparent_comparator_v<C> && is_invocable_r_v<bool, C, T, U></c>.
template <typename C, typename T, typename U = T>
extern constexpr bool is_transparent_comparator_for_v =
    is_transparent_comparator_v<C> && is_invocable_r_v<bool, C, T, U>;

/// @brief Trait to determine if a given type is a _transparent comparator_ for a given set of operands.
///
/// @tparam C The type to test
/// @tparam T The type of the left hand operand
/// @tparam U The type of the right hand operand
/// @var value Will be equivalent to <c>is_transparent_comparator_v<C> && is_invocable_r_v<bool, C, T, U></c>.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename C, typename T, typename U = T>
using is_transparent_comparator_for = std::integral_constant<bool, is_transparent_comparator_for_v<C, T, U>>;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_TRANSPARENT_COMPARATOR_FOR_HPP_
