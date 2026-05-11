// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_NAMED_REQUIREMENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_NAMED_REQUIREMENTS_HPP_

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_object.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

namespace std {
namespace internal {
namespace named_requirement {

/// @brief determine if a type satisfies the MoveAssignable named requirement
/// @tparam T type to check
///
/// Primary template.
///
template <class T, class = arene::base::constraints<>>
extern constexpr bool is_move_assignable_v = false;

/// @brief determine if a type satisfies the MoveAssignable named requirement
/// @tparam T type to check
///
/// Specialization for types that satisfy the named requirement at a syntactic
/// level.
///
template <class T>
extern constexpr bool is_move_assignable_v<
    T,
    arene::base::constraints<std::enable_if_t<std::is_object_v<T>>, std::enable_if_t<std::is_move_assignable_v<T>>>> =
    // some GCC8 bug prevents use of assignment in decltype with the other constraints
    // NOTE: this appears to be fixed with GCC 9+
    // see (maybe): https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63198
    std::is_same_v<T&, decltype(std::declval<T&>() = std::declval<T&&>())>;

}  // namespace named_requirement
}  // namespace internal
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_NAMED_REQUIREMENTS_HPP_
