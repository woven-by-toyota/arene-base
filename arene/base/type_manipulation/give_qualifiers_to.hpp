// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_GIVE_QUALIFIERS_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_GIVE_QUALIFIERS_TO_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/type_manipulation/static_if.hpp"

namespace arene {
namespace base {

namespace give_qualifiers_to_detail {
template <class T>
constexpr bool is_const_v = false;

template <class T>
constexpr bool is_const_v<T const> = true;

template <class T>
constexpr bool is_volatile_v = false;

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "volatile keyword used to check for volatile objects"
template <class T>
constexpr bool is_volatile_v<T volatile> = true;
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "volatile keyword used to preserve qualification of volatile objects"
/// @brief Implementation detail for implementing "GiveQualifiersTo" below.
/// @tparam Source The type providing the qualifiers
/// @tparam Target The type to modify the qualifiers of
///
/// This alias handles the case where Source is volatile
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class Source, class Target>
using give_qualifiers_to_impl_volatile =
    typename static_if<is_const_v<Source>>::template then_else<Target const volatile, Target volatile>;
// parasoft-end-suppress AUTOSAR-A2_7_3
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

/// @brief Implementation detail for implementing "GiveQualifiersTo" below.
/// @tparam Source The type providing the qualifiers
/// @tparam Target The type to modify the qualifiers of
///
/// This alias handles the case where Source is not volatile
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class Source, class Target>
using give_qualifiers_to_impl_nonvolatile =
    typename static_if<is_const_v<Source>>::template then_else<Target const, Target>;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace give_qualifiers_to_detail

/// @brief For a type Source and type Target:
///   if Source is const volatile
///     results in Target const volatile
///   otherwise if Source is volatile
///     results in Target volatile
///   otherwise if Source is const
///     result in Target const
///   otherwise
///     results in Target
/// @tparam Source The type providing the qualifiers
/// @tparam Target The type to modify the qualifiers of
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class Source, class Target>
using give_qualifiers_to =
    typename static_if<give_qualifiers_to_detail::is_volatile_v<Source>>::template then_apply_else_apply<
        give_qualifiers_to_detail::give_qualifiers_to_impl_volatile,
        give_qualifiers_to_detail::give_qualifiers_to_impl_nonvolatile,
        Source,
        Target>;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_GIVE_QUALIFIERS_TO_HPP_
