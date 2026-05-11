// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_PRIORITY_TAG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_PRIORITY_TAG_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"  // IWYU pragma: keep

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Everything is preceeded by a comment with @brief"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: No code is commented out, an example is included in the
// docstring"

/// @brief A priority tag for overload resolution.
/// @tparam N The priority level.
///
/// The priority tag gives an easy to reason about method of controlling the function overload resolution order. It
/// creates an inheritance hierarchy based on the integer given, and overload resultion will rank potential overloads
/// based on the depth of the inheritance hierarchy.
///
/// ~~~{.cpp}
/// // Exact match, first overload tried
/// template<class T>
/// auto test(T t, priority_tag<2>) -> decltype(swap(t, t), std::true_type{});
///
/// // Conversion to parent class, second overload tried
/// template<class T>
/// auto test(T t, priority_tag<1>) -> decltype((t).swap(t), std::true_type{});
///
/// // Conversion to parent-of-parent class, third overload tried
/// template<class T>
/// auto test(T t, priority_tag<0>) -> std::false_type;
///
/// template<class T>
/// using has_swap = decltype(test(T{}, priority_tag<2>{}));
/// ~~~
///
template <std::size_t N>
class priority_tag : public priority_tag<N - std::size_t{1}> {};

// parasoft-end-suppress AUTOSAR-A2_7_2-a "False positive: No code is commented out, an example is included in the
// docstring"

/// @brief A priority tag for overload resolution.
/// @tparam N The priority level.
///
/// The base priority_tag which ends the inheritance hierarchy.
template <>
class priority_tag<std::size_t{0}> {};
/// @}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_PRIORITY_TAG_HPP_
