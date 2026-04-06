// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_HAS_OVERLOADED_ADDRESS_OPERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_HAS_OVERLOADED_ADDRESS_OPERATOR_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_class.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace arene {
namespace base {

namespace has_overloaded_address_operator_detail {
/// @brief Check if a type has the expected return types for the address operator; instantiation fails if @c T does not
/// match the expected signature for @c operator&
/// @tparam T The type to check
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <
    typename T,
    typename = std::enable_if_t<
        std::is_same<decltype(&std::declval<T&>()), T*>::value &&
        std::is_same<decltype(&std::declval<T const&>()), T const*>::value>>
struct has_normal_address_operator_types {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Check if a type has an address operator for non-const values; instantiation fails if @c T does not have an
/// address operator overload that works for non-const objects
/// @tparam T The type to check
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T, typename = decltype(static_cast<T* (T::*)()>(&T::operator&))>
struct has_overloaded_address_operator_for_lvalue {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Check if a type has an address operator for const values; instantiation fails if @c T does not have an
/// address operator overload that works for const objects
/// @tparam T The type to check
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T, typename = decltype(static_cast<T const* (T::*)() const>(&T::operator&))>
struct has_overloaded_address_operator_for_const_lvalue {};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace has_overloaded_address_operator_detail

/// @brief Type trait to check if a type has an overloaded address operator; the value is @c true if the type has an
/// overloaded address operator, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool has_overloaded_address_operator_v =
    std::is_class<T>::value &&
    (!substitution_succeeds<has_overloaded_address_operator_detail::has_normal_address_operator_types, T> ||
     substitution_succeeds<has_overloaded_address_operator_detail::has_overloaded_address_operator_for_lvalue, T> ||
     substitution_succeeds<
         has_overloaded_address_operator_detail::has_overloaded_address_operator_for_const_lvalue,
         T>);

/// @brief Type trait to check if a type has an overloaded address operator; the class derives from @c true_type if the
/// type has an overloaded address operator, @c false_type otherwise
/// @tparam T The type to check
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
class has_overloaded_address_operator : public std::integral_constant<bool, has_overloaded_address_operator_v<T>> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_HAS_OVERLOADED_ADDRESS_OPERATOR_HPP_
